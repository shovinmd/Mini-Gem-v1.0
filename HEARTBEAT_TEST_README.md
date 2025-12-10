# Heartbeat Sensor Test Codes

This folder contains test codes to verify your heartbeat sensor hardware and connections.

## Files

1. **`gpio_hardware_test.ino`** - Basic GPIO connection test
2. **`heartbeat_sensor_test.ino`** - Full heartbeat sensor functionality test

## Hardware Setup

Make sure your heartbeat sensor is connected correctly:

- **GPIO 32** → Heartbeat sensor power input (VCC)
- **GPIO 34** → Heartbeat sensor signal output (analog)
- **GND** → Ground connection
- **3.3V** → Power supply (if needed)

## Testing Steps

### Step 1: Basic Hardware Test

1. Upload `gpio_hardware_test.ino` to your ESP32
2. Open Serial Monitor (115200 baud)
3. Watch the output to verify:
   - GPIO 32 can turn sensor power on/off
   - GPIO 34 reads different values when sensor is on vs off
   - LED flashes correctly

**Expected Output:**
```
=== GPIO Hardware Test ===
GPIO 32 set to HIGH - Heartbeat sensor should be powered
GPIO 32 set to LOW - Heartbeat sensor should be off
Reading 1: 1234 (1.23V)
✓ Heartbeat sensor is responding!
```

### Step 2: Full Functionality Test

1. Upload `heartbeat_sensor_test.ino` to your ESP32
2. Open Serial Monitor (115200 baud)
3. Use these commands:

**Commands:**
- `on` - Turn on heartbeat sensor
- `off` - Turn off heartbeat sensor  
- `test` - Run 30-second automatic test
- `raw` - Show continuous raw values
- `cal` - Calibrate detection threshold

**Example Test Sequence:**
```
on
Place your finger on the sensor...
test
Raw: 2045 | BPM: 72 | LED: ON
Heartbeat detected! BPM: 72
```

## Troubleshooting

### No Response from Sensor
- Check power connections (GPIO 32)
- Verify sensor is getting 3.3V power
- Check signal connection (GPIO 34)
- Make sure sensor is properly seated

### Inconsistent Readings
- Run `cal` command to calibrate threshold
- Adjust `HEARTBEAT_THRESHOLD` value in code
- Make sure finger is making good contact
- Try different finger positions

### BPM Too High/Low
- Adjust `MIN_PEAK_INTERVAL_MS` and `MAX_PEAK_INTERVAL_MS`
- Check if sensor is detecting noise instead of heartbeat
- Verify sensor is designed for finger placement

## Expected Values

- **Raw ADC Values:** 0-4095 (0V to 3.3V)
- **Normal Range:** 1000-3000 (varies by sensor)
- **BPM Range:** 30-200 beats per minute
- **Threshold:** Usually 70% of signal range

## Integration with Main Code

Once testing is complete, the heartbeat sensor should work in your main ESP32 Gemini Assistant code. The main code uses the same GPIO pins and similar detection logic.

## Notes

- Some heartbeat sensors need a few seconds to stabilize after power-on
- Finger placement and pressure affect readings
- Ambient light can interfere with optical sensors
- Make sure sensor is clean and dry
