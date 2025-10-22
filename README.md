# 🤖 ESP32 Gemini Assistant

<div align="center">

![ESP32 Gemini Assistant](https://img.shields.io/badge/ESP32-Gemini%20Assistant-blue?style=for-the-badge&logo=arduino)
![Version](https://img.shields.io/badge/Version-3.0-green?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)

**A sophisticated AI-powered assistant with advanced eye animations, heartbeat monitoring, and smart home integration**

[![Arduino](https://img.shields.io/badge/Arduino-Compatible-red?style=flat&logo=arduino)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green?style=flat&logo=espressif)](https://www.espressif.com/)
[![WiFi](https://img.shields.io/badge/WiFi-Enabled-blue?style=flat&logo=wifi)](https://en.wikipedia.org/wiki/Wi-Fi)

</div>

---

## 📋 Table of Contents

- [🎯 Features](#-features)
- [🔧 Hardware Requirements](#-hardware-requirements)
- [📦 Installation](#-installation)
- [🔌 Wiring Diagram](#-wiring-diagram)
- [⚙️ Configuration](#️-configuration)
- [🚀 Usage](#-usage)
- [🎨 Advanced Features](#-advanced-features)
- [🌐 Web Interface](#-web-interface)
- [🔧 Hardware Testing](#-hardware-testing)
- [🚨 Troubleshooting](#-troubleshooting)
- [📱 Screenshots](#-screenshots)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

---

## 🎯 Features

### 🧠 AI Integration
- **Gemini 2.0 Flash** - Latest Google Gemini AI model for faster, better responses
- **Smart Responses** - Context-aware AI interactions with proper JSON parsing
- **Web-based Interface** - Easy chat interface via browser
- **Error Handling** - Robust error handling for API responses

### 👁️ Advanced Eye Animations
- **13+ Expressions** - Normal, Happy, Sad, Angry, Cute, Wonder, Blink
- **Directional Movement** - Left, Right, Up, Down eye movements
- **Smooth Transitions** - Frame-by-frame animation system
- **Emotional Responses** - Eyes react to different system states
- **Dynamic Timing** - Eye expressions change every 2 seconds for lively interaction

### 👤 User Personalization
- **Custom Username** - Set your name for personalized experience
- **Time-based Greetings** - "Good morning/afternoon/evening/night, [username]"
- **Welcome Messages** - Personalized startup with 3-second display
- **Persistent Settings** - Username saved to SPIFFS, survives restarts
- **Web Interface** - Easy username update via System tab

### 💓 Heartbeat Monitoring
- **Real-time BPM** - Accurate heart rate calculation
- **Peak Detection** - Advanced algorithm for pulse detection
- **LED Synchronization** - 4 LEDs blink with heartbeat
- **Power Management** - Sensor only active when needed

### 📱 Smart Home Control
- **4-LED Lamp System** - Individual and pattern control
- **6 Lighting Patterns** - Off, All On, Wave, Chase, Breathing, Rainbow
- **Brightness Control** - Adjustable LED intensity
- **Touch Navigation** - Intuitive menu system

### ⏰ Task & Alarm Management
- **Task Scheduling** - Time-based task notifications
- **Alarm System** - Custom alarms with messages
- **SPIFFS Storage** - Persistent data storage
- **Web Management** - Full CRUD operations via browser

### 🌐 Network Features
- **WiFi Configuration** - Web-based network setup
- **Network Scanning** - Discover available networks
- **OTA Updates** - Wireless firmware updates
- **RESTful API** - Complete API for all features

---

## 🔧 Hardware Requirements

### 📦 Components List

| Component | Quantity | Specification |
|-----------|----------|---------------|
| **ESP32 Dev Board** | 1 | ESP32-WROOM-32 or similar |
| **SSD1306 OLED Display** | 1 | 128x64, I2C interface |
| **TTP223 Touch Sensor** | 2 | Capacitive touch module (self-lock high TTL mode) |
| **Heartbeat Sensor** | 1 | Analog pulse sensor |
| **LEDs** | 4 | Any color, 3mm or 5mm |
| **Resistors** | 4 | 220Ω (for LEDs) |
| **Passive Buzzer** | 1 | 5V compatible |
| **Breadboard** | 1 | Half-size or full-size |
| **Jumper Wires** | 25+ | Male-to-male, Male-to-female |



---

## 📦 Installation

### 1️⃣ Arduino IDE Setup

```bash
# Install Arduino IDE (2.0.0 or later)
# Download from: https://www.arduino.cc/en/software
```

### 2️⃣ ESP32 Board Package

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. Add this URL to **Additional Board Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for **"ESP32"** and install **ESP32 by Espressif Systems**

### 3️⃣ Required Libraries

Install these libraries through **Tools** → **Manage Libraries**:

| Library | Version | Purpose |
|---------|---------|---------|
| **ArduinoJson** | 6.21.3+ | JSON parsing |
| **U8g2** | 2.34.22+ | OLED graphics |

### 4️⃣ Board Configuration

Select: **Tools** → **Board** → **ESP32 Arduino** → **ESP32 Dev Module**

Configure these settings:
- **Upload Speed**: `115200`
- **CPU Frequency**: `240MHz`
- **Flash Frequency**: `80MHz`
- **Flash Mode**: `QIO`
- **Flash Size**: `4MB`
- **Partition Scheme**: `Default 4MB with spiffs`

---

## 🔌 Wiring Diagram

### 📍 Pin Connections

```
┌─────────────────┐    ┌─────────────────┐
│   ESP32 Board   │    │   Components    │
├─────────────────┤    ├─────────────────┤
│ GPIO21 (SDA)    │◄──►│ OLED SDA        │
│ GPIO22 (SCL)    │◄──►│ OLED SCL        │
│ GPIO4           │◄──►│ Touch Sensor 1  │
│ GPIO5           │◄──►│ Touch Sensor 2  │
│ GPIO34          │◄──►│ Heartbeat Signal│
│ GPIO32          │◄──►│ Heartbeat VCC   │
│ GPIO25          │◄──►│ Main LED + 220Ω │
│ GPIO18          │◄──►│ LED 2 + 220Ω    │
│ GPIO19          │◄──►│ LED 3 + 220Ω    │
│ GPIO23          │◄──►│ LED 4 + 220Ω    │
│ GPIO26          │◄──►│ Buzzer +        │
│ 3.3V            │◄──►│ All VCC pins    │
│ GND             │◄──►│ All GND pins    │
└─────────────────┘    └─────────────────┘
```

### 🔗 Detailed Connections

#### OLED Display (SSD1306)
```
ESP32 Pin    OLED Pin    Function
GPIO21   →   SDA        I2C Data
GPIO22   →   SCL        I2C Clock
3.3V     →   VCC        Power
GND      →   GND        Ground
```

#### Touch Sensors (TTP223) - Dual Sensor Setup
```
ESP32 Pin    TTP223 Pin    Function
GPIO4    →   OUT           Touch Sensor 1 (Open menu and navigate)
GPIO5    →   OUT           Touch Sensor 2 (Move down list, cycle to top)
3.3V     →   VCC           Power (both sensors)
GND      →   GND           Ground (both sensors)
```

#### Heartbeat Sensor
```
ESP32 Pin    Sensor Pin    Function
GPIO34   →   Signal       Analog Reading
GPIO32   →   VCC          Power Control
GND      →   GND          Ground
```

#### LEDs (4x) - Updated Pin Assignments
```
ESP32 Pin    LED Pin       Function
GPIO25   →   LED1 + 220Ω   Main LED
GPIO18   →   LED2 + 220Ω   Additional LED
GPIO19   →   LED3 + 220Ω   Additional LED
GPIO23   →   LED4 + 220Ω   Additional LED
GND      →   All Cathodes  Common Ground
```

#### Passive Buzzer
```
ESP32 Pin    Buzzer Pin    Function
GPIO26   →   Positive      Sound Output
GND      →   Negative      Ground
```

---

## ⚙️ Configuration

### 🔧 Hardware Configuration

All pin definitions are in `config.h`:

```cpp
// OLED Display
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C

// Touch Sensors (Dual Setup)
#define TOUCH_PIN_1 4    // Touch Sensor 1: Open menu and navigate
#define TOUCH_PIN_2 5    // Touch Sensor 2: Move down list, cycle to top

// Heartbeat Sensor
#define HEARTBEAT_PIN 34
#define HEARTBEAT_POWER_PIN 32

// LEDs (Updated Pin Assignments)
#define LED_PIN 25
int ledPins[4] = {25, 18, 19, 23}; // Main LED + 3 additional LEDs

// Buzzer
#define BUZZER_PIN 26
```

### 🌐 WiFi Configuration

Update `config.h` with your network credentials:

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

### 🤖 Gemini API Setup

1. Get API key from [Google AI Studio](https://makersuite.google.com/app/apikey)
2. Update `config.h`:

```cpp
#define GEMINI_API_KEY "YOUR_GEMINI_API_KEY"
```

---

## 🚀 Usage

### 🎮 Dual Touch Sensor Controls

| Sensor | Function | Description |
|--------|----------|-------------|
| **Touch Sensor 1** | Open Menu / Select | Opens menu from idle, selects current item in menu |
| **Touch Sensor 2** | Navigate / Cycle | Moves down through menu items, cycles to top when at end |

### 📱 Menu Navigation System

#### **Menu Flow:**
1. **Touch Sensor 1** → Opens menu (shows first item highlighted)
2. **Touch Sensor 2** → Navigate through menu items
3. **At end of list** → Automatically cycles back to top
4. **Touch Sensor 1** → Select highlighted item and close menu

#### **Menu Options:**
| Option | Description |
|--------|-------------|
| **Dashboard** | Main status screen with time and greetings |
| **Gemini Chat** | AI chat interface |
| **Tasks** | View and manage scheduled tasks |
| **Heartbeat** | Monitor heart rate with LED sync |
| **Settings** | System configuration options |
| **System** | System information and management |

### 🎯 Quick Reference:
- **Open Menu**: Touch Sensor 1 (from idle screen)
- **Navigate**: Touch Sensor 2 (move down through items)
- **Select**: Touch Sensor 1 (choose highlighted item)
- **Cycle**: Touch Sensor 2 (at end of list goes back to top)

---

## 🎨 Advanced Features

### 👁️ Eye Animation System

The assistant features sophisticated eye expressions:

```cpp
// Available animations
normal()     // Default expression
blink()      // Realistic blinking
happy()      // Wide-eyed happiness
sad()        // Droopy with tears
angry()      // Angry eyebrows
cute()       // Cute expression
wonder()     // Wondering look
lefteye()    // Left eye movement
righteye()   // Right eye movement
downeye()    // Downward movement
upeye()      // Upward movement
```

### 💡 LED Pattern System

Control 4 LEDs with 6 different patterns:

| Pattern | Description |
|---------|-------------|
| **Off** | All LEDs off |
| **All On** | All LEDs on |
| **Wave** | Sequential wave pattern |
| **Chase** | Chasing light effect |
| **Breathing** | Smooth breathing effect |
| **Rainbow** | Color cycling pattern |

### 💓 Heartbeat Monitoring

- **Real-time BPM** calculation using peak detection
- **LED synchronization** - All 4 LEDs blink with heartbeat
- **Power management** - Sensor only active in heartbeat menu
- **Visual feedback** on OLED display

---

## 🌐 Web Interface

Access the web interface at `http://ESP32_IP_ADDRESS`:

### 📋 Task Management
- Create, edit, delete tasks
- Time-based scheduling
- Due task notifications

### ⏰ Alarm System
- Set custom alarms
- Test alarm sounds
- Multiple alarm support

### 💡 LED Control
- Individual LED control
- Pattern selection
- Brightness adjustment

### 🌐 WiFi Configuration
- Network scanning
- Credential management
- Automatic reconnection

### 🔄 System Management
- **OTA Updates** - Over-the-air firmware updates
- **Arduino IDE Integration** - Direct upload via network port
- **Web Upload** - Upload .bin files via browser interface
- **Progress Display** - Real-time update progress on OLED
- **Password Protection** - Secure updates (default: "gemini123")
- **Device Restart** - Remote restart functionality
- **Factory Reset** - Complete system reset with data clearing
- **Username Management** - Update personalized username

### 🤖 Gemini Chat
- AI conversation interface
- Real-time responses
- Chat history

---

## 🔧 Hardware Testing

Use `hardware_test.ino` to verify all components:

### 🧪 Test Sequence
1. **OLED Test** - Display verification
2. **Touch Sensor Test** - Touch detection
3. **Heartbeat Sensor Test** - Sensor readings
4. **SPIFFS Test** - File system
5. **WiFi Test** - Network connection
6. **LED Test** - All LEDs functionality
7. **Buzzer Test** - Sound output

### 🎮 Test Navigation
- **Single Tap** - View test details
- **Double Tap** - Next page of results
- **Sequential Testing** - Tests run one by one

---

## 🚨 Troubleshooting

### ❌ Common Issues

#### WiFi Connection Failed
```bash
# Check Serial Monitor output
# Verify SSID and password in config.h
# Use web interface to update WiFi settings
```

#### OLED Not Displaying
```bash
# Check I2C connections (SDA/SCL)
# Verify OLED address (usually 0x3C)
# Ensure U8g2 library is installed
```

#### Touch Sensor Not Working
```bash
# Verify GPIO4 and GPIO5 connections
# Check TTP223 power and ground for both sensors
# Ensure sensors are in self-lock high TTL mode
# Test each sensor individually
```

#### Heartbeat Sensor Issues
```bash
# Ensure analog connection to GPIO34
# Check sensor power control on GPIO32
# Sensor only active in heartbeat menu
```

#### LED Issues
```bash
# Check new LED pin assignments: GPIO 25, 18, 19, 23
# Verify 220Ω resistors are connected
# Ensure GPIO 26 is only used for buzzer (not LED)
# Test each LED individually
```

#### Buzzer Not Working
```bash
# Verify GPIO 26 connection (dedicated buzzer pin)
# Check buzzer polarity and power
# Ensure no conflict with LED pins
# Test with tone() function
```

### 🔍 Debug Information

Enable Serial Monitor (115200 baud) to see:
- WiFi connection status
- SPIFFS initialization
- Dual touch sensor detection events
- Heartbeat readings
- Gemini API responses and debugging
- Animation state changes
- Menu navigation (open, navigate, select)
- LED pattern changes
- Buzzer sound events
- Error messages and troubleshooting info

---

## 📱 Screenshots

### 🏠 Idle Screen
```
┌─────────────────────────┐
│ 14:30              !    │
│                         │
│    ┌─────┐  ┌─────┐     │
│    │  ^  │  │  ^  │     │
│    │     │  │     │     │
│    └─────┘  └─────┘     │
│                         │
│ BPM: 72        Ready   │
└─────────────────────────┘
```

### 📋 Menu Screen
```
┌─────────────────────────┐
│      Main Menu          │
│                         │
│ ► Dashboard             │
│    Gemini Chat          │
│    Tasks                │
│    Heartbeat            │
│    Settings             │
│    System               │
│                         │
│    T1:Select T2:Next    │
└─────────────────────────┘
```

---

## 🆕 Recent Updates (v3.0)

### ✨ New Features Added

#### **Dual Touch Sensor System**
- **Two TTP223 sensors** for intuitive navigation
- **Touch Sensor 1**: Opens menu and selects items
- **Touch Sensor 2**: Navigates through menu items and cycles to top
- **Self-lock high TTL mode** support for both sensors

#### **Enhanced Menu System**
- **6 Menu Options**: Dashboard, Gemini Chat, Tasks, Heartbeat, Settings, System
- **Improved Navigation**: Clear separation between open/select and navigate functions
- **Fixed Text Overlapping**: Proper spacing prevents display issues
- **Better UX**: Intuitive touch controls with clear instructions

#### **Updated Pin Assignments**
- **GPIO Conflict Resolution**: Fixed GPIO 26 conflict between LED and buzzer
- **New LED Pins**: GPIO 18, 19, 23 (instead of 26, 27, 14)
- **Clean Separation**: Each component has dedicated pins
- **Better Organization**: Logical grouping of related functions

#### **Improved Gemini AI Integration**
- **Enhanced API Support**: Primary gemini-1.5-flash with gemini-pro fallback
- **Better Error Handling**: Detailed error messages and HTML response detection
- **Larger Response Buffer**: 8192 bytes for longer AI responses
- **Complete Response Reading**: Timeout-based reading with complete data capture

#### **Advanced Display Features**
- **Time-based Greetings**: "Good morning/afternoon/evening/night, [username]"
- **Date Display**: HH:MM DD/MM format on idle screen
- **Personalized Welcome**: 20-second welcome message with user's name
- **WiFi Status Indicator**: Visual feedback for connection status

#### **Web Interface Improvements**
- **Tab Navigation**: Fixed JavaScript syntax for proper tab switching
- **Time Settings**: Manual time setting and NTP synchronization
- **User Management**: Username update functionality
- **Better Error Handling**: Improved API response handling

### 🔧 Technical Improvements

#### **Code Quality**
- **Modern JavaScript**: Converted to older syntax for better compatibility
- **Error Handling**: Robust error detection and user feedback
- **Debug Output**: Detailed Serial Monitor logging
- **Memory Management**: Optimized buffer sizes and response handling

#### **Hardware Compatibility**
- **Pin Optimization**: No more GPIO conflicts
- **Better Documentation**: Clear pin assignments and wiring diagrams
- **Component Support**: Dual touch sensor setup
- **Power Management**: Efficient sensor power control

#### **User Experience**
- **Intuitive Controls**: Clear touch sensor functions
- **Visual Feedback**: Better display formatting and spacing
- **Personalization**: Custom username and time-based greetings
- **Reliability**: Improved error handling and fallback mechanisms

---

## 🤝 Contributing

We welcome contributions! Please follow these steps:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes (`git commit -m 'Add some AmazingFeature'`)
4. **Push** to the branch (`git push origin feature/AmazingFeature`)
5. **Open** a Pull Request

### 📝 Contribution Guidelines
- Follow Arduino coding standards
- Add comments to complex code
- Test hardware compatibility
- Update documentation

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 Abdulsalam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 👨‍💻 Author

**Abdulsalam**
- GitHub: [@abdulsalam](https://github.com/abdulsalam)
- Project: ESP32 Gemini Assistant v3.0

---

## 🙏 Acknowledgments

- **Google** for Gemini AI API
- **Espressif** for ESP32 platform
- **U8g2** library developers
- **Arduino** community
- **Open source** contributors

---

<div align="center">

**⭐ Star this repository if you found it helpful!**

[![GitHub stars](https://img.shields.io/github/stars/username/esp32-gemini-assistant?style=social)](https://github.com/username/esp32-gemini-assistant)
[![GitHub forks](https://img.shields.io/github/forks/username/esp32-gemini-assistant?style=social)](https://github.com/username/esp32-gemini-assistant)

Made with ❤️ by Abdulsalam

</div>
