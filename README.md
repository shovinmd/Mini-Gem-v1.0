# 🤖 ESP32 Mini Gem v1.0

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
- **Gemini AI Chat** - Conversational AI powered by Google's Gemini
- **Smart Responses** - Context-aware AI interactions
- **Web-based Interface** - Easy chat interface via browser

### 👁️ Advanced Eye Animations
- **13+ Expressions** - Normal, Happy, Sad, Angry, Cute, Wonder, Blink
- **Directional Movement** - Left, Right, Up, Down eye movements
- **Smooth Transitions** - Frame-by-frame animation system
- **Emotional Responses** - Eyes react to different system states

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
| **TTP223 Touch Sensor** | 1 | Capacitive touch module |
| **Heartbeat Sensor** | 1 | Analog pulse sensor |
| **LEDs** | 4 | Any color, 3mm or 5mm |
| **Resistors** | 4 | 220Ω (for LEDs) |
| **Passive Buzzer** | 1 | 5V compatible |
| **Breadboard** | 1 | Half-size or full-size |
| **Jumper Wires** | 20+ | Male-to-male, Male-to-female |

### 💰 Estimated Cost
- **Total Cost**: ~$25-35 USD
- **ESP32**: $8-12
- **OLED Display**: $5-8
- **Sensors & Components**: $10-15

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
│ GPIO4           │◄──►│ Touch Sensor OUT│
│ GPIO34          │◄──►│ Heartbeat Signal│
│ GPIO32          │◄──►│ Heartbeat VCC   │
│ GPIO25          │◄──►│ LED 1 + 220Ω    │
│ GPIO26          │◄──►│ LED 2 + 220Ω    │
│ GPIO27          │◄──►│ LED 3 + 220Ω    │
│ GPIO14          │◄──►│ LED 4 + 220Ω    │
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

#### Touch Sensor (TTP223)
```
ESP32 Pin    TTP223 Pin    Function
GPIO4    →   OUT           Touch Detection
3.3V     →   VCC           Power
GND      →   GND           Ground
```

#### Heartbeat Sensor
```
ESP32 Pin    Sensor Pin    Function
GPIO34   →   Signal       Analog Reading
GPIO32   →   VCC          Power Control
GND      →   GND          Ground
```

#### LEDs (4x)
```
ESP32 Pin    LED Pin       Function
GPIO25   →   LED1 + 220Ω   Main LED
GPIO26   →   LED2 + 220Ω   Additional LED
GPIO27   →   LED3 + 220Ω   Additional LED
GPIO14   →   LED4 + 220Ω   Additional LED
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

### 🎮 Touch Controls

| Gesture | Function |
|---------|----------|
| **Single Tap** | Wake animation / Select menu option |
| **Double Tap** | Show notifications / Navigate menu |
| **Triple Tap** | Open main menu |

### 📱 Menu Navigation

1. **Triple Tap** - Open main menu
2. **Double Tap** - Navigate: Tasks → Heartbeat → Gemini → Lamp → Alarm
3. **Single Tap** - Select current option

### 🎯 Menu Options

| Option | Description |
|--------|-------------|
| **Tasks** | View and manage scheduled tasks |
| **Heartbeat** | Monitor heart rate with LED sync |
| **Gemini** | AI chat interface |
| **Lamp** | Control 4 LEDs with patterns |
| **Alarm** | Set and manage alarms |

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
- OTA firmware updates
- Device restart
- Factory reset

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
# Verify GPIO4 connection
# Check TTP223 power and ground
# Adjust sensitivity if needed
```

#### Heartbeat Sensor Issues
```bash
# Ensure analog connection to GPIO34
# Check sensor power control on GPIO32
# Sensor only active in heartbeat menu
```

#### Compilation Errors
```bash
# Ensure all libraries are installed
# Check #include statements
# Verify config.h file exists
# Check Arduino IDE version compatibility
```

### 🔍 Debug Information

Enable Serial Monitor (115200 baud) to see:
- WiFi connection status
- SPIFFS initialization
- Touch detection events
- Heartbeat readings
- API responses
- Animation state changes
- Menu navigation
- Error messages

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
│ ► 1. Tasks              │
│    2. Heartbeat         │
│    3. Gemini            │
│    4. Lamp              │
│    5. Alarm             │
│                         │
│    Tap:Select 2Tap:Next │
└─────────────────────────┘
```

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
