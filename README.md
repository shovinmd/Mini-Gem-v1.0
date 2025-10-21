# ESP32 Gemini Assistant - Complete Setup Guide

## 🚀 Project Overview
A sophisticated ESP32-based AI assistant with advanced eye animations, heartbeat monitoring, task management, alarm system, WiFi configuration, and Gemini AI integration. Features a responsive web interface and OLED display with touch controls.

## 📚 Required Libraries
Install these libraries through the Arduino IDE Library Manager:

### Core Libraries
1. **WiFi** (ESP32 Core) - Built-in with ESP32 board package
2. **WebServer** (ESP32 Core) - Built-in with ESP32 board package
3. **SPIFFS** (ESP32 Core) - Built-in with ESP32 board package
4. **Wire** (Arduino Core) - Built-in Arduino library for I2C communication
5. **HTTPClient** (ESP32 Core) - Built-in with ESP32 board package
6. **WiFiClientSecure** (ESP32 Core) - Built-in with ESP32 board package

### Third-Party Libraries
7. **ArduinoJson** by Benoit Blanchon
   - Version: 6.21.3 or later
   - Used for JSON parsing and generation

8. **U8g2** by Oliver Kraus
   - Version: 2.34.22 or later
   - Advanced graphics library for OLED displays
   - **Note**: We use U8g2 instead of Adafruit libraries for better performance

## 🔧 Hardware Requirements

### Components
- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **SSD1306 OLED Display** (128x64, I2C)
- **TTP223 Touch Sensor Module**
- **Heartbeat/Pulse Sensor** (analog output)
- **4x LEDs** (any color) with 220Ω resistors
- **Passive Buzzer** (for sound feedback)
- **Breadboard and jumper wires**

### Pin Connections
```
ESP32 Pin    Component                Function
GPIO21       OLED SDA                 I2C Data Line
GPIO22       OLED SCL                 I2C Clock Line
GPIO4        TTP223 OUT               Touch Detection
GPIO34       Heartbeat Sensor         Analog Reading
GPIO32       Heartbeat Sensor VCC     Power Control
GPIO25       LED 1 + Resistor         Main LED
GPIO26       LED 2 + Resistor         Additional LED
GPIO27       LED 3 + Resistor         Additional LED
GPIO14       LED 4 + Resistor         Additional LED
GPIO26       Passive Buzzer           Sound Output
3.3V         Power for sensors        VCC
GND          Ground for all components Common Ground
```

## ⚙️ Configuration

### WiFi Setup
Update these variables in `config.h`:
```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

### Gemini API Setup
1. Get API key from Google AI Studio: https://makersuite.google.com/app/apikey
2. Update the API key in `config.h`:
```cpp
#define GEMINI_API_KEY "YOUR_GEMINI_API_KEY"
```

### Hardware Configuration
All pin definitions and settings are centralized in `config.h`:
```cpp
// OLED Display
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C

// Touch Sensor
#define TOUCH_PIN 4

// Heartbeat Sensor
#define HEARTBEAT_PIN 34
#define HEARTBEAT_POWER_PIN 32

// LEDs
#define LED_PIN 25
int ledPins[4] = {25, 26, 27, 14};

// Buzzer
#define BUZZER_PIN 26
```

## 📦 Installation Instructions

### 1. Arduino IDE Setup
1. Install Arduino IDE (2.0.0 or later)
2. Install ESP32 board package:
   - File → Preferences
   - Add URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

### 2. Library Installation
Install all required libraries through Library Manager:
- ArduinoJson
- U8g2

### 3. Board Configuration
Select board: Tools → Board → ESP32 Arduino → ESP32 Dev Module
Configure upload settings:
- **Upload Speed**: 115200
- **CPU Frequency**: 240MHz
- **Flash Frequency**: 80MHz
- **Flash Mode**: QIO
- **Flash Size**: 4MB
- **Partition Scheme**: Default 4MB with spiffs

### 4. File Structure
Ensure your project folder contains:
```
esp32_gemini_assistant/
├── esp32_gemini_assistant.ino
├── config.h
├── animations.h
├── hardware_test.ino
├── README.md
└── CIRCUIT_DIAGRAM.md
```

### 5. Upload Code
1. Connect ESP32 via USB
2. Select correct COM port
3. Upload the main sketch
4. Upload SPIFFS data (if needed)

## 🎮 Usage Guide

### Touch Controls
- **Single Tap**: Wake animation / Select menu option
- **Double Tap**: Show notifications / Navigate menu
- **Triple Tap**: Open main menu

### Menu Navigation
1. **Triple Tap** - Open main menu
2. **Double Tap** - Navigate: Tasks → Heartbeat → Gemini → Lamp → Alarm
3. **Single Tap** - Select current option

### Menu Options
1. **Tasks** - View and manage tasks
2. **Heartbeat** - Monitor heart rate with LED sync
3. **Gemini** - AI chat interface
4. **Lamp** - Control 4 LEDs with patterns
5. **Alarm** - Set and manage alarms

### Web Interface
Access via ESP32 IP address for:
- **Task Management** - Create, edit, delete tasks
- **Alarm Settings** - Set alarms with custom messages
- **LED Control** - Individual LED control
- **WiFi Configuration** - Change WiFi settings
- **System Management** - OTA updates, restart, factory reset
- **Gemini Chat** - AI conversation interface

## 🎨 Advanced Features

### Eye Animations
Sophisticated eye expressions using U8g2:
- **Normal** - Default expression
- **Blink** - Realistic blinking
- **Happy** - Wide-eyed happiness
- **Sad** - Droopy with tears
- **Angry** - Angry eyebrows
- **Cute** - Cute expression
- **Wonder** - Wondering look
- **Left/Right Eye** - Directional movement
- **Up/Down Eye** - Vertical movement

### LED Patterns
4-LED control with patterns:
- **Off** - All LEDs off
- **All On** - All LEDs on
- **Wave** - Wave pattern
- **Chase** - Chasing pattern
- **Breathing** - Breathing effect
- **Rainbow** - Color cycling

### Heartbeat Monitoring
- **Real-time BPM** calculation
- **Peak detection** algorithm
- **LED synchronization** with heartbeat
- **Power control** - Sensor only active when needed
- **Visual feedback** on OLED

### Alarm System
- **Time-based alarms** stored in SPIFFS
- **Custom messages** for each alarm
- **Sound testing** functionality
- **Multiple alarms** support
- **Web interface** management

### WiFi Management
- **Network scanning** - Discover available networks
- **Credential storage** - Save WiFi settings to SPIFFS
- **Automatic reconnection** - Load saved credentials on startup
- **Web-based configuration** - Change settings remotely

## 🔧 Hardware Testing

### Test Sketch
Use `hardware_test.ino` to verify all components:
1. **OLED Test** - Display test patterns
2. **Touch Sensor Test** - Verify touch detection
3. **Heartbeat Sensor Test** - Check sensor readings
4. **SPIFFS Test** - Verify file system
5. **WiFi Test** - Test network connection
6. **LED Test** - Verify all LEDs work
7. **Buzzer Test** - Test sound output

### Test Navigation
- **Single Tap** - View test details
- **Double Tap** - Next page of results
- **Sequential Testing** - Tests run one by one

## 🚨 Troubleshooting

### Common Issues

#### WiFi Connection Failed
- Check SSID and password in `config.h`
- Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Use web interface to update WiFi settings

#### OLED Not Displaying
- Check I2C connections (SDA/SCL)
- Verify OLED address (usually 0x3C)
- Check power connections
- Ensure U8g2 library is installed

#### Touch Sensor Not Working
- Verify GPIO4 connection
- Check TTP223 power and ground
- Adjust sensitivity if needed

#### Heartbeat Sensor Issues
- Ensure analog connection to GPIO34
- Check sensor power control on GPIO32
- Verify sensor output voltage range
- Sensor only active in heartbeat menu

#### LED/Buzzer Not Working
- Check pin connections
- Verify resistors are properly connected
- Test individual components with hardware test

#### Gemini API Errors
- Verify API key is correct
- Check internet connection
- Ensure HTTPS is working
- Check API quota limits

#### Compilation Errors
- Ensure all libraries are installed
- Check `#include` statements
- Verify `config.h` file exists
- Check Arduino IDE version compatibility

### Serial Monitor Output
Enable Serial Monitor (115200 baud) to see:
- WiFi connection status
- SPIFFS initialization
- Touch detection events
- Heartbeat readings
- API responses
- Animation state changes
- Menu navigation
- Error messages

### Debug Information
The system provides detailed logging:
- **Startup sequence** - Component initialization
- **Network status** - WiFi connection details
- **Sensor readings** - Heartbeat and touch data
- **Menu navigation** - State changes
- **API calls** - Gemini requests/responses
- **File operations** - SPIFFS read/write

## 🔄 OTA Updates
- Access OTA interface at `http://ESP32_IP/update`
- Upload new firmware wirelessly
- No physical connection required
- Automatic restart after update

## 📱 Mobile Compatibility
The web interface is fully responsive and works on:
- **Smartphones** - Touch-friendly interface
- **Tablets** - Optimized layout
- **Desktop** - Full feature access
- **Cross-platform** - Works on all modern browsers

## 🎯 Performance Optimization
- **Power management** - Heartbeat sensor only active when needed
- **Memory efficient** - Optimized animations and data structures
- **Fast response** - Minimal delays in touch detection
- **Smooth animations** - Frame-rate optimized eye expressions
- **Efficient storage** - JSON-based data management

## 📞 Support
For issues and questions:
1. Check Serial Monitor output
2. Run hardware test sketch
3. Verify all connections
4. Check library versions
5. Review configuration settings

## 🔮 Future Enhancements
- Voice recognition integration
- Additional sensor support
- Cloud synchronization
- Advanced AI features
- Mobile app development
- Multi-language support

---

**Created by**: Abdulsalam  
**Version**: 3.0  
**Last Updated**: 2024  
**License**: Open Source"# Mini-Gem-v1.0" 
