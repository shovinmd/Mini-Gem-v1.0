# ESP32 Gemini Assistant - Circuit Diagram

## Hardware Components Required

### Main Components
- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **SSD1306 OLED Display** (128x64, I2C interface)
- **TTP223 Touch Sensor Module**
- **Heartbeat/Pulse Sensor** (analog output type)
- **LED** (any color, 3mm or 5mm)
- **220Ω Resistor** (for LED current limiting)
- **Breadboard** (half-size or full-size)
- **Jumper Wires** (male-to-male, male-to-female)

### Optional Components
- **Power Supply** (5V, 2A recommended)
- **USB Cable** (for programming and power)
- **Enclosure/Case** (3D printed or purchased)

## Pin Connections

### ESP32 Pinout Reference
```
ESP32 Pin Layout (top view):
                   3V3  [1]  [2]  GND
                   EN   [3]  [4]  GPIO23
                   VP   [5]  [6]  GPIO22  ← SCL
                   VN   [7]  [8]  GPIO1
                   GPIO34 [9]  [10] GPIO3
                   GPIO35 [11] [12] GPIO21 ← SDA
                   GPIO32 [13] [14] GND
                   GPIO33 [15] [16] GPIO25 ← LED
                   GPIO27 [17] [18] GPIO26
                   GPIO14 [19] [20] GPIO4  ← Touch
                   GPIO12 [21] [22] GPIO0
                   GND   [23] [24] GPIO2
                   GPIO13 [25] [26] GPIO5
                   D2    [27] [28] GPIO18
                   D3    [29] [30] GPIO19
                   CMD   [31] [32] GPIO16
                   SD0   [33] [34] GPIO17
                   SD1   [35] [36] GPIO15
                   SD2   [37] [38] GPIO14
                   SD3   [39] [40] GPIO12
```

### Connection Table

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| **OLED Display** | | |
| SDA | GPIO21 | I2C Data |
| SCL | GPIO22 | I2C Clock |
| VCC | 3.3V | Power |
| GND | GND | Ground |
| **Touch Sensor** | | |
| OUT | GPIO4 | Digital Input |
| VCC | 3.3V | Power |
| GND | GND | Ground |
| **Heartbeat Sensor** | | |
| Signal | GPIO34 | Analog Input (ADC) |
| VCC | 3.3V | Power |
| GND | GND | Ground |
| **LED Indicator** | | |
| Anode | GPIO25 | Digital Output |
| Cathode | GND | Through 220Ω resistor |

## Circuit Diagram (ASCII Art)

```
                    ESP32 Development Board
                    ┌─────────────────────┐
                    │ 3V3  EN  VP  VN     │
                    │ GPIO34 GPIO35 GPIO32 │
                    │ GPIO33 GPIO25 GPIO26 │
                    │ GPIO27 GPIO4  GPIO0  │
                    │ GPIO12 GPIO2  GPIO5  │
                    │ GPIO13 GPIO18 GPIO19 │
                    │ GPIO16 GPIO17 GPIO15 │
                    │ GPIO14 GPIO12 GPIO15 │
                    │ GND   GND   GND      │
                    └─────────────────────┘
                           │ │ │ │ │
                           │ │ │ │ └─ GPIO25 ──[220Ω]── LED ── GND
                           │ │ │ └─── GPIO4  ── Touch Sensor OUT
                           │ │ └───── GPIO34 ── Heartbeat Sensor Signal
                           │ └─────── GPIO22 ── OLED SCL
                           └───────── GPIO21 ── OLED SDA

    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
    │   OLED     │    │   Touch    │    │ Heartbeat   │
    │  Display   │    │  Sensor    │    │   Sensor    │
    │            │    │            │    │            │
    │ SDA ───────┼────┼────────────┼────┼────────────┼─── GPIO21
    │ SCL ───────┼────┼────────────┼────┼────────────┼─── GPIO22
    │ VCC ───────┼────┼────────────┼────┼────────────┼─── 3.3V
    │ GND ───────┼────┼────────────┼────┼────────────┼─── GND
    └────────────┘    │            │    │            │
                      │ OUT ───────┼────┼────────────┼─── GPIO4
                      │ VCC ───────┼────┼────────────┼─── 3.3V
                      │ GND ───────┼────┼────────────┼─── GND
                      └────────────┘    │            │
                                        │ Signal ────┼─── GPIO34
                                        │ VCC ────────┼─── 3.3V
                                        │ GND ────────┼─── GND
                                        └────────────┘
```

## Wiring Instructions

### Step 1: Power Connections
1. Connect ESP32 **3.3V** pin to the positive rail of the breadboard
2. Connect ESP32 **GND** pin to the negative rail of the breadboard
3. Connect all component VCC pins to the positive rail
4. Connect all component GND pins to the negative rail

### Step 2: OLED Display
1. Connect OLED **SDA** to ESP32 **GPIO21**
2. Connect OLED **SCL** to ESP32 **GPIO22**
3. Connect OLED **VCC** to 3.3V rail
4. Connect OLED **GND** to GND rail

### Step 3: Touch Sensor
1. Connect TTP223 **OUT** to ESP32 **GPIO4**
2. Connect TTP223 **VCC** to 3.3V rail
3. Connect TTP223 **GND** to GND rail

### Step 4: Heartbeat Sensor
1. Connect heartbeat sensor **Signal** to ESP32 **GPIO34**
2. Connect heartbeat sensor **VCC** to 3.3V rail
3. Connect heartbeat sensor **GND** to GND rail

### Step 5: LED Indicator
1. Connect one end of 220Ω resistor to ESP32 **GPIO25**
2. Connect other end of resistor to LED anode (longer leg)
3. Connect LED cathode (shorter leg) to GND rail

## Power Requirements

### ESP32 Power Consumption
- **Active Mode**: ~240mA @ 3.3V
- **WiFi Active**: ~160mA additional
- **Deep Sleep**: ~10μA

### Component Power Consumption
- **OLED Display**: ~20mA @ 3.3V
- **Touch Sensor**: ~1mA @ 3.3V
- **Heartbeat Sensor**: ~5mA @ 3.3V
- **LED**: ~10mA @ 3.3V (with 220Ω resistor)

### Total Power Requirements
- **Peak Load**: ~435mA @ 3.3V
- **Recommended Supply**: 5V, 2A (with voltage regulator)

## Safety Considerations

### Electrical Safety
1. **Double-check all connections** before powering on
2. **Use proper resistor values** for LED current limiting
3. **Avoid short circuits** between power and ground
4. **Check voltage levels** - ESP32 is 3.3V logic

### Component Protection
1. **ESD Protection**: Handle components with care
2. **Heat Management**: Ensure adequate ventilation
3. **Mechanical Stress**: Secure connections properly

## Troubleshooting

### Common Issues
1. **OLED Not Displaying**
   - Check I2C connections (SDA/SCL)
   - Verify OLED address (usually 0x3C)
   - Check power connections

2. **Touch Sensor Not Working**
   - Verify GPIO4 connection
   - Check TTP223 power and ground
   - Test with multimeter

3. **Heartbeat Sensor Issues**
   - Ensure analog connection to GPIO34
   - Check sensor power (3.3V)
   - Verify sensor output voltage range

4. **LED Not Lighting**
   - Check resistor value (220Ω)
   - Verify LED polarity
   - Test GPIO25 output

### Testing Procedures
1. **Power Test**: Measure 3.3V on power rail
2. **I2C Test**: Check SDA/SCL voltages
3. **GPIO Test**: Use multimeter to verify pin states
4. **Sensor Test**: Monitor analog readings

## Enclosure Design

### 3D Printing Considerations
- **Material**: PLA or PETG recommended
- **Wall Thickness**: 2-3mm minimum
- **Ventilation**: Include holes for heat dissipation
- **Access**: Design removable panels for programming

### Mounting Options
- **Standalone**: Desktop display unit
- **Wall Mount**: Fixed installation
- **Portable**: Battery-powered version

## Future Enhancements

### Additional Sensors
- **Temperature/Humidity**: DHT22 or BME280
- **Light Sensor**: LDR or TSL2561
- **Motion Sensor**: PIR sensor
- **Sound Sensor**: Microphone module

### Connectivity Options
- **Bluetooth**: ESP32 built-in Bluetooth
- **LoRa**: Long-range communication
- **Ethernet**: Wired network connection
- **Cellular**: GSM/LTE module

### Display Upgrades
- **Color OLED**: RGB display
- **E-Paper**: Low-power display
- **TFT LCD**: Larger color screen
- **LED Matrix**: Custom animations
