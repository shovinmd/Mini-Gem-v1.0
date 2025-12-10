# ESP32 SPIFFS Web Interface Setup

This guide explains how to set up the ESP32 Gemini Assistant to serve the web interface from SPIFFS instead of embedded HTML.

## What is SPIFFS?

SPIFFS (SPI Flash File System) is a file system for microcontrollers that allows you to store files in flash memory. This approach has several advantages:

- **Smaller sketch size**: The HTML/CSS/JS code is stored separately from the Arduino sketch
- **Easier updates**: You can update the web interface without recompiling the entire sketch
- **Better organization**: Web files are separate from code logic
- **Faster loading**: Files are served directly from flash memory

## Setup Instructions

### Method 1: Using Arduino IDE (Recommended)

1. **Install ESP32 Board Package**:
   - Open Arduino IDE
   - Go to `File > Preferences`
   - Add this URL to Additional Board Manager URLs:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to `Tools > Board > Boards Manager`
   - Search for "ESP32" and install the package

2. **Install SPIFFS Data Upload Tool**:
   - Go to `Tools > Board > Boards Manager`
   - Search for "SPIFFS" and install "ESP32 Sketch Data Upload"

3. **Upload the Sketch**:
   - Connect your ESP32 to your computer
   - Select your ESP32 board: `Tools > Board > ESP32 Arduino > [Your Board]`
   - Select the correct port: `Tools > Port > [Your Port]`
   - Open `esp32_gemini_assistant.ino`
   - Click `Upload` (or press Ctrl+U)

4. **Upload SPIFFS Data**:
   - After the sketch upload completes, go to `Tools > ESP32 Sketch Data Upload`
   - Wait for the upload to complete
   - The ESP32 will restart automatically

### Method 2: Using Arduino CLI

If you have Arduino CLI installed:

```bash
# Compile the sketch
arduino-cli compile --fqbn esp32:esp32:esp32 esp32_gemini_assistant

# Upload the sketch
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 esp32_gemini_assistant

# Upload SPIFFS data
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 --input-dir esp32_gemini_assistant/data esp32_gemini_assistant
```

Replace `COM3` with your actual port (e.g., `/dev/ttyUSB0` on Linux/Mac).

### Method 3: Using the Batch Script

On Windows, you can use the provided batch script:

1. Double-click `upload_spiffs.bat`
2. Follow the on-screen instructions
3. The script will open Arduino IDE for you

## File Structure

```
esp32_gemini_assistant/
├── esp32_gemini_assistant.ino    # Main Arduino sketch
├── config.h                      # Configuration file
├── data/                         # SPIFFS data directory
│   └── web_interface.html        # Web interface file
├── upload_spiffs.bat             # Windows upload script
└── upload_web_interface.py       # Python upload script
```

## How It Works

1. **Sketch Upload**: The Arduino sketch is compiled and uploaded to the ESP32
2. **SPIFFS Upload**: The `data/` directory contents are uploaded to SPIFFS
3. **Web Server**: The ESP32 serves files from SPIFFS when requested

The web server code in `esp32_gemini_assistant.ino` has been modified to:
- Check if `web_interface.html` exists in SPIFFS
- Serve the file from SPIFFS if found
- Fall back to embedded HTML if SPIFFS file is not found

## Troubleshooting

### SPIFFS Upload Fails

- **Check connections**: Ensure ESP32 is properly connected
- **Check port**: Make sure you're using the correct COM port
- **Check board**: Verify you've selected the correct ESP32 board
- **Try manual upload**: Use Arduino IDE's SPIFFS Data Upload tool manually

### Web Interface Not Loading

- **Check serial monitor**: Look for "SPIFFS file served successfully" message
- **Check fallback**: If you see "SPIFFS file not found", the fallback HTML is being used
- **Re-upload SPIFFS**: Try uploading the SPIFFS data again

### File Not Found Errors

- **Check file path**: Ensure `web_interface.html` is in the `data/` directory
- **Check file name**: The file must be named exactly `web_interface.html`
- **Check file size**: Very large files might not fit in SPIFFS

## Benefits of This Approach

1. **Modularity**: Web interface is separate from code logic
2. **Maintainability**: Easier to update the web interface
3. **Performance**: Faster loading from flash memory
4. **Flexibility**: Can add more web files (CSS, JS, images) easily
5. **Version Control**: Web files can be version controlled separately

## Updating the Web Interface

To update the web interface:

1. Edit `esp32_gemini_assistant/data/web_interface.html`
2. Upload only the SPIFFS data (no need to recompile the sketch)
3. Use `Tools > ESP32 Sketch Data Upload` in Arduino IDE

## File Size Limits

- **SPIFFS Size**: Typically 1.5MB for ESP32
- **Current Web Interface**: ~15KB (well within limits)
- **Room for Growth**: Plenty of space for additional files

## Next Steps

Once SPIFFS is set up, you can:

1. Add CSS files for better styling
2. Add JavaScript libraries
3. Add images and icons
4. Create multiple HTML pages
5. Implement a proper file structure

The ESP32 will automatically serve these files from SPIFFS, making your web interface more professional and maintainable.
