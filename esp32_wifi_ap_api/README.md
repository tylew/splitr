# ESP32 WiFi Access Point with HTTP API for BLE Audio Control

This project sets up an ESP32 to:
1. Create a WiFi Access Point
2. Run an HTTP API server
3. Communicate with an nRF5340 over UART to control BLE audio broadcasting

## Overview

The ESP32 serves as a network-accessible control hub for the audio broadcasting system. It:
- Creates a WiFi access point that users can connect to
- Provides HTTP API endpoints to control the nRF5340 BLE audio broadcaster
- Communicates with the nRF5340 via UART connection

## WiFi Access Point Details

- **SSID**: ESP32-Access-Point
- **Password**: 123456789
- **Maximum Connections**: 4
- **Authentication Mode**: WPA/WPA2 PSK

## HTTP API Endpoints

The ESP32 provides the following API endpoints:

### 1. GET /api/data
- Returns a simple "Hello, World!" JSON message
- Used for testing connectivity

### 2. POST /start-broadcast
- Sends a "START" command to the nRF5340 over UART
- Instructs the nRF5340 to begin BLE audio broadcasting (Auracast)
- Returns a JSON status response

Example response:
```json
{
  "status": "success",
  "message": "BLE broadcast started"
}
```

### 3. POST /stop-broadcast
- Sends a "STOP" command to the nRF5340 over UART
- Instructs the nRF5340 to stop BLE audio broadcasting
- Returns a JSON status response

Example response:
```json
{
  "status": "success",
  "message": "BLE broadcast stopped"
}
```

## UART Configuration for nRF5340 Communication

To connect the ESP32 to the nRF5340, use the following UART configuration:

- **UART Port**: UART1
- **TX Pin**: GPIO 17 (ESP32) → RX (nRF5340)
- **RX Pin**: GPIO 16 (ESP32) → TX (nRF5340)
- **Baud Rate**: 115200
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Flow Control**: Disabled

## Command Format

The ESP32 sends simple text commands to the nRF5340:
- `START\n` - Begin BLE broadcasting
- `STOP\n` - End BLE broadcasting

## Development Notes

For testing without the physical nRF5340 hardware, the code includes a mock function that simulates successful responses from the nRF5340.

## Building and Running

```bash
# Build the project
idf.py build

# Flash to ESP32
idf.py -p PORT flash

# Monitor output
idf.py -p PORT monitor
```

Or use the provided buildflash.sh script:
```bash
./buildflash.sh
```

# _Sample project_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This is the simplest buildable example. The example is used by command `idf.py create-project`
that copies the project to user specified path and set it's name. For more information follow the [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project)



## How to use example
We encourage the users to use the example as a template for the new projects.
A recommended way is to follow the instructions on a [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project).

## Example folder contents

The project **sample_project** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
