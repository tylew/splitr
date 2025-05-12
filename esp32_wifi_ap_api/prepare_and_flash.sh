#!/bin/bash
set -e

# Source ESP-IDF environment
echo "Setting up ESP-IDF environment..."
source "$HOME/Downloads/SP 25/CPSC 514/esp/esp-idf/export.sh"

# Create data directory if it doesn't exist
if [ ! -d "data" ]; then
  echo "Creating data directory..."
  mkdir -p data
fi

# Check if sample.mp3 exists in data directory
if [ ! -f "data/sample.mp3" ]; then
  echo "Warning: No sample.mp3 file found in data directory!"
  echo "Please copy an MP3 file to data/sample.mp3 before continuing."
  echo "Example: cp /path/to/your/music.mp3 data/sample.mp3"
  
  # Ask user if they want to continue anyway
  read -p "Continue without sample.mp3? (y/n) " -n 1 -r
  echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Exiting. Please add an MP3 file and try again."
    exit 1
  fi
else
  echo "Found sample.mp3 in data directory."
fi

# Build the project
echo "Building the project..."
idf.py build

# Flash the firmware
echo "Flashing firmware..."
read -p "Enter the ESP32 port (e.g., /dev/tty.wchusbserial59590744071): " PORT
idf.py -p "$PORT" flash

# Flash SPIFFS partition with data directory contents
if [ -f "data/sample.mp3" ]; then
  echo "Flashing SPIFFS partition with data directory contents..."
  idf.py -p "$PORT" spiffs-flash
fi

echo "Done! You can now monitor the ESP32:"
echo "idf.py -p $PORT monitor" 