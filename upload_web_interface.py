#!/usr/bin/env python3
"""
ESP32 SPIFFS Web Interface Uploader
Uploads the web_interface.html file to ESP32 SPIFFS filesystem
"""

import serial
import time
import sys
import os

def upload_to_spiffs(port, baudrate=115200):
    """Upload web interface file to ESP32 SPIFFS"""
    
    # Check if web interface file exists
    web_file = "esp32_gemini_assistant/web_interface.html"
    if not os.path.exists(web_file):
        print(f"Error: {web_file} not found!")
        return False
    
    try:
        # Open serial connection
        ser = serial.Serial(port, baudrate, timeout=10)
        print(f"Connected to {port} at {baudrate} baud")
        
        # Wait for ESP32 to be ready
        time.sleep(2)
        
        # Send command to enter SPIFFS upload mode
        ser.write(b"SPIFFS_UPLOAD\n")
        time.sleep(1)
        
        # Read web interface file
        with open(web_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Send file size first
        file_size = len(content.encode('utf-8'))
        ser.write(f"FILE_SIZE:{file_size}\n".encode())
        time.sleep(0.5)
        
        # Send file content
        print(f"Uploading {file_size} bytes...")
        ser.write(content.encode('utf-8'))
        
        # Wait for confirmation
        time.sleep(2)
        response = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
        print(f"ESP32 Response: {response}")
        
        ser.close()
        print("Upload completed!")
        return True
        
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False

def main():
    """Main function"""
    if len(sys.argv) != 2:
        print("Usage: python upload_web_interface.py <COM_PORT>")
        print("Example: python upload_web_interface.py COM3")
        print("         python upload_web_interface.py /dev/ttyUSB0")
        return
    
    port = sys.argv[1]
    success = upload_to_spiffs(port)
    
    if success:
        print("✅ Web interface uploaded successfully!")
        print("The ESP32 will now serve the web interface from SPIFFS")
    else:
        print("❌ Upload failed!")

if __name__ == "__main__":
    main()
