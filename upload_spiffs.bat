@echo off
echo ESP32 SPIFFS Web Interface Uploader
echo ===================================
echo.

REM Check if Arduino IDE is installed
set ARDUINO_IDE_PATH="C:\Program Files (x86)\Arduino\arduino.exe"
if not exist %ARDUINO_IDE_PATH% (
    set ARDUINO_IDE_PATH="C:\Program Files\Arduino\arduino.exe"
)

if not exist %ARDUINO_IDE_PATH% (
    echo Error: Arduino IDE not found!
    echo Please install Arduino IDE or update the path in this script.
    pause
    exit /b 1
)

echo Found Arduino IDE at: %ARDUINO_IDE_PATH%
echo.

REM Check if data directory exists
if not exist "esp32_gemini_assistant\data" (
    echo Error: Data directory not found!
    echo Please make sure you're running this script from the project root directory.
    pause
    exit /b 1
)

echo Data directory found: esp32_gemini_assistant\data
echo.

REM Check if web interface file exists
if not exist "esp32_gemini_assistant\data\web_interface.html" (
    echo Error: web_interface.html not found in data directory!
    pause
    exit /b 1
)

echo Web interface file found: esp32_gemini_assistant\data\web_interface.html
echo.

echo Instructions:
echo 1. Connect your ESP32 to your computer
echo 2. Open Arduino IDE
echo 3. Install ESP32 board package if not already installed
echo 4. Install SPIFFS Data Upload tool:
echo    - Go to Tools ^> Board ^> Boards Manager
echo    - Search for "ESP32" and install
echo    - Go to Tools ^> Board and select your ESP32 board
echo    - Go to Tools ^> Port and select your ESP32 port
echo 5. Install SPIFFS Data Upload tool:
echo    - Go to Tools ^> Board ^> Boards Manager
echo    - Search for "SPIFFS" and install "ESP32 Sketch Data Upload"
echo 6. Upload the sketch first: File ^> Upload
echo 7. Then upload SPIFFS data: Tools ^> ESP32 Sketch Data Upload
echo.

echo Alternative method using Arduino CLI (if installed):
echo arduino-cli compile --fqbn esp32:esp32:esp32 esp32_gemini_assistant
echo arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 esp32_gemini_assistant
echo arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 --input-dir esp32_gemini_assistant/data esp32_gemini_assistant
echo.

echo Press any key to continue...
pause > nul

echo.
echo Opening Arduino IDE...
start "" %ARDUINO_IDE_PATH% "esp32_gemini_assistant\esp32_gemini_assistant.ino"

echo.
echo Done! Follow the instructions above to upload your sketch and SPIFFS data.
pause
