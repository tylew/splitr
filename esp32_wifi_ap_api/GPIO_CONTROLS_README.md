# ESP32 Audio Player GPIO Controls

This document provides instructions for testing the GPIO button controls on your ESP32 audio player.

## Connecting the GPIO Buttons

Connect the buttons to the ESP32 pins as follows:

1. **Play/Pause Button**: Connect to GPIO 32
2. **Stop Button**: Connect to GPIO 33
3. **Volume Up Button**: Connect to GPIO 25
4. **Volume Down Button**: Connect to GPIO 26

Each button should be connected between the GPIO pin and GND (ground). The code uses internal pull-up resistors, so no external resistors are needed.

## Hardware Connection Diagram

```md
         +--------+
         |  ESP32 |
         +--------+
         |        |
GPIO 32 -|---o/o--|--- GND  (Play/Pause Button)
         |        |
GPIO 33 -|---o/o--|--- GND  (Stop Button)
         |        |
GPIO 25 -|---o/o--|--- GND  (Volume Up Button)
         |        |
GPIO 26 -|---o/o--|--- GND  (Volume Down Button)
         |        |
         +--------+
```

## Testing the GPIO Controls

1. **Build and Flash the Project:**

- Run the provided script:

```sh
./prepare_and_flash.sh
```

- The script will guide you through:
   - Adding a sample MP3 file to the data directory
   - Building the project
   - Flashing the firmware to your ESP32
   - Uploading the MP3 file to the ESP32's SPIFFS filesystem

2. **Monitor the ESP32's Output:**

- After flashing, connect to the ESP32's serial monitor:

```sh
idf.py -p [PORT] monitor
```

- Replace `[PORT]` with your ESP32's serial port (e.g., /dev/tty.wchusbserial59590744071)

- You should see log messages when you press the buttons

3. **Test Each Button:**

   - **Play/Pause Button (GPIO 32)**: Press to toggle between play and pause
   - **Stop Button (GPIO 33)**: Press to stop playback
   - **Volume Up Button (GPIO 25)**: Press to increase volume
   - **Volume Down Button (GPIO 26)**: Press to decrease volume

## Troubleshooting

If the buttons aren't working as expected:

1. **Check Connections**: Ensure the buttons are properly connected to the correct GPIO pins and GND
2. **Verify Button Presses**: The ESP32 should log button press events in the serial monitor
3. **Check MP3 File**: Ensure your MP3 file was successfully uploaded to SPIFFS
4. **Restart ESP32**: Try power cycling the ESP32 if necessary

## Memory Usage Notes

The ESP32 has limited DRAM, so we've:

- Removed the embedded MP3 sample from the code
- Implemented file-based playback using the SPIFFS filesystem
- Added debounce logic to prevent button press issues

These changes ensure stable operation while still providing all the required functionality.