/*
 * Heartbeat Sensor Test Code
 * Tests the heartbeat sensor functionality
 * 
 * Hardware:
 * - Heartbeat Sensor Power: GPIO 32
 * - Heartbeat Sensor Signal: GPIO 34 (ADC)
 * - LED Indicator: GPIO 25 (built-in LED)
 * - Serial Monitor for output
 */

// Pin definitions
#define HEARTBEAT_POWER_PIN 32  // Power control for heartbeat sensor
#define HEARTBEAT_PIN 34        // Analog input from heartbeat sensor
#define LED_PIN 25              // Built-in LED for visual feedback

// Heartbeat detection variables
int heartbeatBuffer[20];
int bufferIndex = 0;
unsigned long lastPeakTime = 0;
int currentBPM = 0;
bool ledState = false;

// Detection thresholds
#define HEARTBEAT_THRESHOLD 2000  // Adjust this value based on your sensor
#define MIN_PEAK_INTERVAL_MS 300  // Minimum 200 BPM
#define MAX_PEAK_INTERVAL_MS 2000 // Minimum 30 BPM

void setup() {
  Serial.begin(115200);
  Serial.println("=== Heartbeat Sensor Test ===");
  
  // Initialize pins
  pinMode(HEARTBEAT_POWER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(HEARTBEAT_PIN, INPUT);
  
  // Start with heartbeat sensor OFF
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize heartbeat buffer
  for(int i = 0; i < 20; i++) {
    heartbeatBuffer[i] = 0;
  }
  
  Serial.println("Heartbeat sensor test initialized");
  Serial.println("Commands:");
  Serial.println("  'on'  - Turn on heartbeat sensor");
  Serial.println("  'off' - Turn off heartbeat sensor");
  Serial.println("  'test' - Run automatic test");
  Serial.println("  'raw' - Show raw sensor values");
  Serial.println("  'cal' - Calibrate threshold");
}

void loop() {
  // Handle serial commands
  if(Serial.available()) {
    String command = Serial.readString();
    command.trim();
    command.toLowerCase();
    
    if(command == "on") {
      turnOnHeartbeatSensor();
    }
    else if(command == "off") {
      turnOffHeartbeatSensor();
    }
    else if(command == "test") {
      runAutomaticTest();
    }
    else if(command == "raw") {
      showRawValues();
    }
    else if(command == "cal") {
      calibrateThreshold();
    }
    else {
      Serial.println("Unknown command. Use: on, off, test, raw, cal");
    }
  }
  
  // Update heartbeat monitoring if sensor is on
  updateHeartbeatSensor();
  
  delay(50); // Small delay for stability
}

void turnOnHeartbeatSensor() {
  digitalWrite(HEARTBEAT_POWER_PIN, HIGH);
  Serial.println("Heartbeat sensor powered ON");
  Serial.println("Place your finger on the sensor...");
  Serial.println("Watch for LED flashes and BPM readings");
}

void turnOffHeartbeatSensor() {
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  Serial.println("Heartbeat sensor powered OFF");
  currentBPM = 0;
}

void runAutomaticTest() {
  Serial.println("=== Running Automatic Test ===");
  
  // Turn on sensor
  digitalWrite(HEARTBEAT_POWER_PIN, HIGH);
  Serial.println("Sensor powered on...");
  delay(2000); // Wait for sensor to stabilize
  
  Serial.println("Place your finger on the sensor now!");
  Serial.println("Test will run for 30 seconds...");
  
  unsigned long testStart = millis();
  unsigned long lastPrint = 0;
  
  while(millis() - testStart < 30000) { // 30 second test
    updateHeartbeatSensor();
    
    // Print status every 2 seconds
    if(millis() - lastPrint > 2000) {
      int rawValue = analogRead(HEARTBEAT_PIN);
      Serial.print("Raw: ");
      Serial.print(rawValue);
      Serial.print(" | BPM: ");
      Serial.print(currentBPM);
      Serial.print(" | LED: ");
      Serial.println(ledState ? "ON" : "OFF");
      lastPrint = millis();
    }
    
    delay(50);
  }
  
  // Turn off sensor
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  Serial.println("Test complete! Sensor powered off.");
  Serial.print("Final BPM reading: ");
  Serial.println(currentBPM);
}

void showRawValues() {
  Serial.println("=== Raw Sensor Values ===");
  Serial.println("Turn on sensor first with 'on' command");
  Serial.println("Press any key to stop...");
  
  while(!Serial.available()) {
    int rawValue = analogRead(HEARTBEAT_PIN);
    Serial.print("Raw ADC Value: ");
    Serial.print(rawValue);
    Serial.print(" | Voltage: ");
    Serial.print((rawValue * 3.3) / 4095.0);
    Serial.println("V");
    delay(100);
  }
  
  // Clear serial buffer
  while(Serial.available()) {
    Serial.read();
  }
}

void calibrateThreshold() {
  Serial.println("=== Threshold Calibration ===");
  Serial.println("Turn on sensor first with 'on' command");
  Serial.println("Place finger on sensor and press any key to start calibration...");
  
  while(!Serial.available()) {
    delay(100);
  }
  
  // Clear serial buffer
  while(Serial.available()) {
    Serial.read();
  }
  
  Serial.println("Calibrating for 10 seconds...");
  Serial.println("Keep finger steady on sensor!");
  
  int minValue = 4095;
  int maxValue = 0;
  unsigned long calStart = millis();
  
  while(millis() - calStart < 10000) { // 10 second calibration
    int rawValue = analogRead(HEARTBEAT_PIN);
    
    if(rawValue < minValue) minValue = rawValue;
    if(rawValue > maxValue) maxValue = rawValue;
    
    Serial.print("Min: ");
    Serial.print(minValue);
    Serial.print(" | Max: ");
    Serial.print(maxValue);
    Serial.print(" | Current: ");
    Serial.println(rawValue);
    
    delay(100);
  }
  
  int threshold = minValue + ((maxValue - minValue) * 0.7); // 70% of range
  
  Serial.println("=== Calibration Results ===");
  Serial.print("Min Value: ");
  Serial.println(minValue);
  Serial.print("Max Value: ");
  Serial.println(maxValue);
  Serial.print("Recommended Threshold: ");
  Serial.println(threshold);
  Serial.println("Update HEARTBEAT_THRESHOLD in code with this value");
}

void updateHeartbeatSensor() {
  // Only update if sensor is powered on
  if(digitalRead(HEARTBEAT_POWER_PIN) == LOW) {
    return;
  }
  
  int sensorValue = analogRead(HEARTBEAT_PIN);
  heartbeatBuffer[bufferIndex] = sensorValue;
  bufferIndex = (bufferIndex + 1) % 20;
  
  // Simple peak detection
  if(detectPeak()) {
    unsigned long currentTime = millis();
    if(lastPeakTime > 0) {
      int interval = currentTime - lastPeakTime;
      if(interval > MIN_PEAK_INTERVAL_MS && interval < MAX_PEAK_INTERVAL_MS) {
        currentBPM = 60000 / interval;
        currentBPM = constrain(currentBPM, 30, 200); // Clamp to reasonable range
        
        // Flash LED with heartbeat
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        
        // Print BPM
        Serial.print("Heartbeat detected! BPM: ");
        Serial.println(currentBPM);
      }
    }
    lastPeakTime = currentTime;
  }
}

bool detectPeak() {
  int current = heartbeatBuffer[bufferIndex];
  int previous = heartbeatBuffer[(bufferIndex - 1 + 20) % 20];
  int next = heartbeatBuffer[(bufferIndex + 1) % 20];
  
  return (current > previous && current > next && current > HEARTBEAT_THRESHOLD);
}
