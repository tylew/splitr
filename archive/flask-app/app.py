from flask import Flask, jsonify, request, render_template
import bluetooth
import subprocess
import pygame
import time

app = Flask(__name__, 
            template_folder='src/templates', 
            static_folder='scr/static')

# Global variable to keep track of the tone process
tone_process = None

# In-memory storage for connected devices (for simplicity)
connected_devices = {}

@app.route('/')
def index():
    """Serve the front-end HTML."""
    return render_template('index.html')

@app.route('/play-tone', methods=['POST'])
def play_tone():
    global tone_process
    data = request.json
    frequency = data.get('frequency', 440)

    try:
        # Stop any existing tone process before starting a new one
        if tone_process and tone_process.poll() is None:
            stop_tone()

        # Start a new tone process
        tone_process = subprocess.Popen(['python3', 'play_audio.py', str(frequency)])

        return jsonify({"message": f"Playing {frequency}Hz tone indefinitely"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/stop-tone', methods=['POST'])
def stop_tone():
    global tone_process
    try:
        if tone_process:
            tone_process.terminate()  # Gracefully terminate the process
            tone_process.wait()  # Wait for the process to fully terminate
            tone_process = None  # Reset the global variable
        return jsonify({"message": "Tone stopped"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/scan', methods=['GET'])
def scan_devices():
    """Scan for nearby Bluetooth devices."""
    nearby_devices = bluetooth.discover_devices(lookup_names=True)
    devices = [{"address": addr, "name": name} for addr, name in nearby_devices]
    return jsonify(devices)

def bluetoothctl(command):
    """Run a command in bluetoothctl and return the output."""
    process = subprocess.Popen(['/usr/bin/bluetoothctl'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate(input=command.encode('utf-8'))
    return stdout.decode('utf-8'), stderr.decode('utf-8')

@app.route('/connect', methods=['POST'])
def connect_device():
    """Connect to a Bluetooth device."""
    data = request.json
    address = data.get('address')
    
    if address in connected_devices:
        return jsonify({"error": "Device already connected"}), 400

    try:
        # Pair with the device
        stdout, stderr = bluetoothctl(f"pair {address}")
        if "Failed" in stderr:
            return jsonify({"error": f"Failed to pair with {address}: {stderr}"}), 500

        # Trust the device
        stdout, stderr = bluetoothctl(f"trust {address}")
        if "Failed" in stderr:
            return jsonify({"error": f"Failed to trust {address}: {stderr}"}), 500

        # Connect to the device
        stdout, stderr = bluetoothctl(f"connect {address}")
        if "Failed" in stderr:
            return jsonify({"error": f"Failed to connect to {address}: {stderr}"}), 500

        # Store the connected device
        connected_devices[address] = {
            "address": address,
            "name": bluetooth.lookup_name(address)
        }

        return jsonify({"message": f"Connected to {address}"}), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/disconnect', methods=['POST'])
def disconnect_device():
    """Disconnect from a Bluetooth device."""
    data = request.json
    address = data.get('address')
    
    if address not in connected_devices:
        return jsonify({"error": "Device not connected"}), 400

    # Simulate disconnection (replace with actual disconnection logic)
    try:
        connected_devices[address]['socket'].close()
        del connected_devices[address]
        return jsonify({"message": f"Disconnected from {address}"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/read', methods=['GET'])
def read_data():
    """Read data from a connected Bluetooth device."""
    address = request.args.get('address')
    
    if address not in connected_devices:
        return jsonify({"error": "Device not connected"}), 400

    # Simulate reading data (replace with actual read logic)
    try:
        data = connected_devices[address]['socket'].recv(1024)  # Read up to 1024 bytes
        return jsonify({"data": data.decode()}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/write', methods=['POST'])
def write_data():
    """Write data to a connected Bluetooth device."""
    data = request.json
    address = data.get('address')
    message = data.get('message')
    
    if address not in connected_devices:
        return jsonify({"error": "Device not connected"}), 400

    # Simulate writing data (replace with actual write logic)
    try:
        connected_devices[address]['socket'].send(message)
        return jsonify({"message": "Data sent successfully"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/connections', methods=['GET'])
def list_connections():
    """List all currently connected Bluetooth devices."""
    connections_info = [
        {"address": addr, "name": info["name"]}
        for addr, info in connected_devices.items()
    ]
    return jsonify(connections_info)

@app.route('/device', methods=['GET'])
def device():
    """List all currently connected Bluetooth devices."""
    device_info = [
        {"name": 'aititan-rpi1', "ip": 'http://192.168.2.2:5000'}
    ]
    return jsonify(device_info)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
