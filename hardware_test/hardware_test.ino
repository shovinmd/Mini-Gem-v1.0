/*
 * ESP32 Hardware Test Suite v3.0
 * Tests all components before running the main application
 * 
 * Components tested:
 * - OLED Display (SSD1306, 128x64)
 * - Touch Sensor 1 (TTP223) - GPIO4
 * - Touch Sensor 2 (TTP223) - GPIO5
 * - Heartbeat Sensor (Analog) - GPIO34 + Power Control GPIO32
 * - LED Indicators (4x) - GPIO25, 18, 19, 23
 * - Passive Buzzer - GPIO26
 * - SPIFFS Storage
 * - WiFi Connection
 */

#include <Wire.h>
#include <U8g2lib.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "../config.h"

// Display object
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Test results - Updated for dual touch sensors and 4 LEDs
bool testResults[8] = {false, false, false, false, false, false, false, false};
const char* testNames[] = {
  "OLED Display",
  "Touch Sensor 1", 
  "Touch Sensor 2",
  "Heartbeat Sensor",
  "LED Indicators",
  "Passive Buzzer",
  "SPIFFS Storage",
  "WiFi Connection"
};

// Test counters - Updated for dual touch sensors
int touchCount1 = 0;
int touchCount2 = 0;
int blinkCount = 0;
int beepCount = 0;
int heartbeatReadings = 0;

// Menu navigation
int currentMenuIndex = 0;
int currentPage = 0;
bool testsCompleted = false;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("\n=== ESP32 Hardware Test Suite ===");
  Serial.println("Testing all components...");
  
  // Initialize pins - Updated for dual touch sensors and new LED pins
  pinMode(TOUCH_PIN_1, INPUT);
  pinMode(TOUCH_PIN_2, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(18, OUTPUT);  // LED 2
  pinMode(19, OUTPUT);  // LED 3
  pinMode(23, OUTPUT);  // LED 4
  pinMode(HEARTBEAT_PIN, INPUT);
  pinMode(HEARTBEAT_POWER_PIN, OUTPUT);  // Heartbeat sensor power control
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Start with heartbeat sensor OFF
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  
  // Initialize I2C for OLED
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  
  // Initialize display
  u8g2.begin();
  
  // Initialize SPIFFS
  SPIFFS.begin(true);
  
  // Run all tests
  runAllTests();
  
  // Display results
  displayTestResults();
  
  testsCompleted = true;
  
  Serial.println("\nHardware test completed!");
  Serial.println("Check the OLED display for results.");
}

void loop() {
  if(testsCompleted) {
    // Show test results menu with touch navigation
    handleTestResultsMenu();
  }
  delay(50);
}

void runAllTests() {
  Serial.println("\nStarting hardware tests...");
  
  // Test 1: OLED Display
  showTestProgress(1, 8, "OLED Display");
  delay(1000);
  testOLED();
  delay(2000);
  
  // Test 2: Touch Sensor 1
  showTestProgress(2, 8, "Touch Sensor 1");
  delay(1000);
  testTouchSensor1();
  delay(2000);
  
  // Test 3: Touch Sensor 2
  showTestProgress(3, 8, "Touch Sensor 2");
  delay(1000);
  testTouchSensor2();
  delay(2000);
  
  // Test 4: Heartbeat Sensor
  showTestProgress(4, 8, "Heartbeat Sensor");
  delay(1000);
  testHeartbeatSensor();
  delay(2000);
  
  // Test 5: LED Indicators
  showTestProgress(5, 8, "LED Indicators");
  delay(1000);
  testLEDs();
  delay(2000);
  
  // Test 6: Passive Buzzer
  showTestProgress(6, 8, "Passive Buzzer");
  delay(1000);
  testBuzzer();
  delay(2000);
  
  // Test 7: SPIFFS Storage
  showTestProgress(7, 8, "SPIFFS Storage");
  delay(1000);
  testSPIFFS();
  delay(2000);
  
  // Test 8: WiFi Connection
  showTestProgress(8, 8, "WiFi Connection");
  delay(1000);
  testWiFi();
  delay(2000);
  
  Serial.println("\nAll tests completed!");
}

void showTestProgress(int current, int total, const char* testName) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  char progress[30];
  sprintf(progress, "Test %d of %d", current, total);
  
  u8g2.drawStr(10, 20, progress);
  u8g2.drawStr(10, 35, testName);
  u8g2.drawStr(10, 50, "Starting...");
  
  u8g2.sendBuffer();
}

void testOLED() {
  Serial.println("Testing OLED Display...");
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  bool passed = true;
  
  // Test basic display functionality
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 20, "OLED Test");
  u8g2.drawStr(10, 35, "Testing...");
  u8g2.sendBuffer();
  
  delay(1000);
  
  // Test different fonts and positions
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  u8g2.drawStr(5, 15, "Font Test");
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(5, 30, "Small Font");
  u8g2.setFont(u8g2_font_10x20_tf);
  u8g2.drawStr(5, 55, "Large");
  u8g2.sendBuffer();
  
  delay(1000);
  
  // Test full screen utilization
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  char resolution[25];
  sprintf(resolution, "Resolution: %dx%d", SCREEN_WIDTH, SCREEN_HEIGHT);
  
  u8g2.drawStr(5, 12, "Full Screen Test");
  u8g2.drawStr(5, 25, resolution);
  u8g2.drawStr(5, 38, "Top Line");
  u8g2.drawStr(5, 51, "Middle Line");
  u8g2.drawStr(5, 64, "Bottom Line");
  
  u8g2.sendBuffer();
  
  delay(1000);
  
  // Test result
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  u8g2.drawStr(10, 30, "OLED: PASSED");
  u8g2.sendBuffer();
  
  testResults[0] = passed;
  Serial.println("OLED Display: PASS");
}

void testTouchSensor1() {
  Serial.println("Testing Touch Sensor 1 (GPIO4)...");
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  digitalWrite(23, LOW);
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Ensure heartbeat sensor is OFF
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  touchCount1 = 0;
  bool passed = false;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 15, "Touch Sensor 1");
  u8g2.drawStr(10, 30, "Pin: GPIO4");
  u8g2.drawStr(10, 45, "Tap sensor 5x");
  u8g2.drawStr(10, 60, "Count: 0/5");
  u8g2.sendBuffer();
  
  int initialTouch = digitalRead(TOUCH_PIN_1);
  unsigned long startTime = millis();
  unsigned long lastTouchTime = 0;
  
  while(millis() - startTime < 10000) { // 10 second timeout
    bool currentTouch = (digitalRead(TOUCH_PIN_1) == HIGH);
    
    if(currentTouch && millis() - lastTouchTime > 200) {
      touchCount1++;
      lastTouchTime = millis();
      
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.drawStr(10, 15, "Touch Sensor 1");
      u8g2.drawStr(10, 30, "Pin: GPIO4");
      u8g2.drawStr(10, 45, "Tap sensor 5x");
      
      char countStr[20];
      sprintf(countStr, "Count: %d/5", touchCount1);
      u8g2.drawStr(10, 60, countStr);
      
      u8g2.sendBuffer();
      
      if(touchCount1 >= 5) {
        passed = true;
        break;
      }
    }
    
    delay(50);
  }
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  digitalWrite(23, LOW);
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Ensure heartbeat sensor is OFF
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  if(passed) {
    u8g2.drawStr(10, 30, "Touch 1: PASSED");
  } else {
    u8g2.drawStr(10, 30, "Touch 1: FAILED");
  }
  u8g2.sendBuffer();
  
  testResults[1] = passed;
  Serial.printf("Touch Sensor 1: %s (Count: %d)\n", passed ? "PASS" : "FAIL", touchCount1);
}

void testTouchSensor2() {
  Serial.println("Testing Touch Sensor 2 (GPIO5)...");
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  digitalWrite(23, LOW);
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Ensure heartbeat sensor is OFF
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  touchCount2 = 0;
  bool passed = false;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 15, "Touch Sensor 2");
  u8g2.drawStr(10, 30, "Pin: GPIO5");
  u8g2.drawStr(10, 45, "Tap sensor 5x");
  u8g2.drawStr(10, 60, "Count: 0/5");
  u8g2.sendBuffer();
  
  int initialTouch = digitalRead(TOUCH_PIN_2);
  unsigned long startTime = millis();
  unsigned long lastTouchTime = 0;
  
  while(millis() - startTime < 10000) { // 10 second timeout
    bool currentTouch = (digitalRead(TOUCH_PIN_2) == HIGH);
    
    if(currentTouch && millis() - lastTouchTime > 200) {
      touchCount2++;
      lastTouchTime = millis();
      
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.drawStr(10, 15, "Touch Sensor 2");
      u8g2.drawStr(10, 30, "Pin: GPIO5");
      u8g2.drawStr(10, 45, "Tap sensor 5x");
      
      char countStr[20];
      sprintf(countStr, "Count: %d/5", touchCount2);
      u8g2.drawStr(10, 60, countStr);
      
      u8g2.sendBuffer();
      
      if(touchCount2 >= 5) {
        passed = true;
        break;
      }
    }
    
    delay(50);
  }
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  digitalWrite(23, LOW);
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Ensure heartbeat sensor is OFF
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  if(passed) {
    u8g2.drawStr(10, 30, "Touch 2: PASSED");
  } else {
    u8g2.drawStr(10, 30, "Touch 2: FAILED");
  }
  u8g2.sendBuffer();
  
  testResults[2] = passed;
  Serial.printf("Touch Sensor 2: %s (Count: %d)\n", passed ? "PASS" : "FAIL", touchCount2);
}

void testHeartbeatSensor() {
  Serial.println("Testing Heartbeat Sensor...");
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  digitalWrite(23, LOW);
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Ensure heartbeat sensor is OFF
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  heartbeatReadings = 0;
  bool passed = false;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  char pinStr[20];
  sprintf(pinStr, "Pin: GPIO%d", HEARTBEAT_PIN);
  
  u8g2.drawStr(10, 15, "Heartbeat Test");
  u8g2.drawStr(10, 30, pinStr);
  u8g2.drawStr(10, 45, "Powering ON...");
  u8g2.sendBuffer();
  
  // Power ON the heartbeat sensor
  digitalWrite(HEARTBEAT_POWER_PIN, HIGH);
  delay(1000); // Give sensor time to stabilize
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 15, "Heartbeat Test");
  u8g2.drawStr(10, 30, pinStr);
  u8g2.drawStr(10, 45, "Reading...");
  u8g2.sendBuffer();
  
  unsigned long startTime = millis();
  int firstReading = analogRead(HEARTBEAT_PIN);
  int lastReading = firstReading;
  bool hasVariation = false;
  
  while(millis() - startTime < 3000) { // 3 second test
    int reading = analogRead(HEARTBEAT_PIN);
    heartbeatReadings++;
    
    // Check if reading is different from first reading
    if(abs(reading - firstReading) > 10) {
      hasVariation = true;
    }
    
    // Display current reading
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(10, 15, "Heartbeat Test");
    u8g2.drawStr(10, 30, pinStr);
    
    char readingStr[25];
    sprintf(readingStr, "Value: %d", reading);
    u8g2.drawStr(10, 45, readingStr);
    
    char countStr[20];
    sprintf(countStr, "Readings: %d", heartbeatReadings);
    u8g2.drawStr(10, 60, countStr);
    
    u8g2.sendBuffer();
    
    delay(100);
  }
  
  // Power OFF the heartbeat sensor
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  
  // Pass if sensor is reading values and shows some variation
  passed = (heartbeatReadings > 0) && hasVariation;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  if(passed) {
    u8g2.drawStr(10, 30, "Heartbeat: PASS");
  } else {
    u8g2.drawStr(10, 30, "Heartbeat: FAIL");
  }
  u8g2.sendBuffer();
  
  testResults[3] = passed;
  Serial.printf("Heartbeat Sensor: %s (Readings: %d, Variation: %s)\n", 
                passed ? "PASS" : "FAIL", heartbeatReadings, hasVariation ? "YES" : "NO");
}

void testLEDs() {
  Serial.println("Testing LED Indicators (4x)...");
  
  blinkCount = 0;
  bool passed = false;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  u8g2.drawStr(10, 15, "LED Test (4x)");
  u8g2.drawStr(10, 30, "Pins: 25,18,19,23");
  u8g2.drawStr(10, 45, "Testing...");
  u8g2.sendBuffer();
  
  // Test all LEDs individually
  int ledPins[] = {LED_PIN, 18, 19, 23};
  const char* ledNames[] = {"LED1", "LED2", "LED3", "LED4"};
  
  for(int led = 0; led < 4; led++) {
    // Ensure all LEDs start OFF
    for(int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    delay(100);
    
    // Test current LED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(10, 15, "LED Test (4x)");
    u8g2.drawStr(10, 30, ledNames[led]);
    
    char pinStr[20];
    sprintf(pinStr, "Pin: GPIO%d", ledPins[led]);
    u8g2.drawStr(10, 45, pinStr);
    
    u8g2.sendBuffer();
    delay(500);
    
    // Blink current LED 3 times
    for(int i = 0; i < 3; i++) {
      digitalWrite(ledPins[led], HIGH);
      delay(300);
      digitalWrite(ledPins[led], LOW);
      delay(300);
      blinkCount++;
    }
    
    delay(500);
  }
  
  // Test all LEDs together
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 15, "LED Test (4x)");
  u8g2.drawStr(10, 30, "All LEDs");
  u8g2.drawStr(10, 45, "Together");
  u8g2.sendBuffer();
  
  // All LEDs ON
  for(int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  delay(1000);
  
  // All LEDs OFF
  for(int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  delay(500);
  
  // Ensure all LEDs are OFF at the end
  for(int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  
  passed = (blinkCount >= 12); // Should have at least 12 blinks (3 per LED)
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  if(passed) {
    u8g2.drawStr(10, 30, "LEDs: PASSED");
  } else {
    u8g2.drawStr(10, 30, "LEDs: FAILED");
  }
  u8g2.sendBuffer();
  
  testResults[4] = passed;
  Serial.printf("LED Indicators: %s (Blinks: %d)\n", passed ? "PASS" : "FAIL", blinkCount);
}

void testBuzzer() {
  Serial.println("Testing Passive Buzzer...");
  
  beepCount = 0;
  bool passed = false;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  char pinStr[20];
  sprintf(pinStr, "Pin: GPIO%d", BUZZER_PIN);
  
  u8g2.drawStr(10, 15, "Buzzer Test");
  u8g2.drawStr(10, 30, pinStr);
  u8g2.drawStr(10, 45, "Testing...");
  u8g2.sendBuffer();
  
  // Test 1: Digital on/off
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 15, "Buzzer Test");
  u8g2.drawStr(10, 30, "Method 1: Digital");
  u8g2.drawStr(10, 45, "ON/OFF");
  u8g2.sendBuffer();
  
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
  beepCount++;
  
  // Test 2: Tone function
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 15, "Buzzer Test");
  u8g2.drawStr(10, 30, "Method 2: Tone");
  u8g2.drawStr(10, 45, "1000Hz");
  u8g2.sendBuffer();
  
  tone(BUZZER_PIN, 1000);
  delay(500);
  noTone(BUZZER_PIN);
  delay(200);
  beepCount++;
  
  // Test 3: Different frequencies
  int frequencies[] = {500, 1000, 1500, 2000};
  for(int i = 0; i < 4; i++) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(10, 15, "Buzzer Test");
    u8g2.drawStr(10, 30, "Method 3: Freq");
    
    char freqStr[20];
    sprintf(freqStr, "%dHz", frequencies[i]);
    u8g2.drawStr(10, 45, freqStr);
    
    u8g2.sendBuffer();
    
    tone(BUZZER_PIN, frequencies[i]);
    delay(300);
    noTone(BUZZER_PIN);
    delay(100);
    beepCount++;
  }
  
  // Test 4: Melody pattern
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 15, "Buzzer Test");
  u8g2.drawStr(10, 30, "Method 4: Melody");
  u8g2.drawStr(10, 45, "Pattern");
  u8g2.sendBuffer();
  
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523}; // C major scale
  for(int i = 0; i < 8; i++) {
    tone(BUZZER_PIN, melody[i]);
    delay(200);
    noTone(BUZZER_PIN);
    delay(50);
    beepCount++;
  }
  
  // Ensure buzzer is OFF
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
  
  passed = (beepCount >= 10); // Should have at least 10 beeps
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  if(passed) {
    u8g2.drawStr(10, 30, "Buzzer: PASSED");
  } else {
    u8g2.drawStr(10, 30, "Buzzer: FAILED");
  }
  u8g2.sendBuffer();
  
  testResults[4] = passed;
  Serial.printf("Passive Buzzer: %s (Beeps: %d)\n", passed ? "PASS" : "FAIL", beepCount);
}

void testSPIFFS() {
  Serial.println("Testing SPIFFS Storage...");
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  bool passed = false;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 20, "SPIFFS Test");
  u8g2.drawStr(10, 35, "Testing...");
  u8g2.sendBuffer();
  
  // Test file operations
  File testFile = SPIFFS.open("/test.txt", "w");
  if(testFile) {
    testFile.println("ESP32 Hardware Test");
    testFile.println("SPIFFS is working!");
    testFile.close();
    
    // Read back the file
    testFile = SPIFFS.open("/test.txt", "r");
    if(testFile) {
      String content = testFile.readString();
      testFile.close();
      
      if(content.indexOf("ESP32 Hardware Test") >= 0) {
        passed = true;
      }
    }
    
    // Clean up test file
    SPIFFS.remove("/test.txt");
  }
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  if(passed) {
    u8g2.drawStr(10, 30, "SPIFFS: PASSED");
  } else {
    u8g2.drawStr(10, 30, "SPIFFS: FAILED");
  }
  u8g2.sendBuffer();
  
  testResults[5] = passed;
  Serial.printf("SPIFFS Storage: %s\n", passed ? "PASS" : "FAIL");
}

void testWiFi() {
  Serial.println("Testing WiFi Connection...");
  
  // Cleanup other components
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
  
  bool passed = false;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(10, 20, "WiFi Test");
  u8g2.drawStr(10, 35, "Connecting...");
  u8g2.sendBuffer();
  
  // Check if WiFi credentials are set
  if(strcmp(WIFI_SSID, "YOUR_WIFI_SSID") == 0) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_8x13_tf);
    u8g2.drawStr(10, 30, "WiFi: NO CREDS");
    u8g2.sendBuffer();
    
    testResults[6] = false;
    Serial.println("WiFi Connection: FAIL (No credentials)");
    return;
  }
  
  // Attempt to connect
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startTime = millis();
  while(WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_CONNECT_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    passed = true;
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(10, 15, "WiFi: CONNECTED");
    u8g2.drawStr(10, 30, "IP:");
    
    String ipStr = WiFi.localIP().toString();
    u8g2.drawStr(10, 45, ipStr.c_str());
    
    u8g2.sendBuffer();
  } else {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_8x13_tf);
    u8g2.drawStr(10, 30, "WiFi: FAILED");
    u8g2.sendBuffer();
  }
  
  testResults[6] = passed;
  Serial.printf("WiFi Connection: %s\n", passed ? "PASS" : "FAIL");
  
  // Disconnect WiFi to free resources
  WiFi.disconnect();
}

void displayTestResults() {
  Serial.println("\n=== TEST RESULTS ===");
  
  int passCount = 0;
  for(int i = 0; i < 8; i++) {
    Serial.printf("%s: %s\n", testNames[i], testResults[i] ? "PASS" : "FAIL");
    if(testResults[i]) passCount++;
  }
  
  Serial.printf("\nOverall Score: %d/8\n", passCount);
  
  if(passCount == 8) {
    Serial.println("✓ Hardware setup looks good!");
    Serial.println("You can now upload the main application.");
  } else {
    Serial.println("⚠ Some tests failed. Check connections and try again.");
  }
  
  Serial.println("\nHardware test completed!");
  Serial.println("Check the OLED display for results.");
}

void handleTestResultsMenu() {
  static unsigned long lastTouchTime = 0;
  static bool touchPressed = false;
  static int tapCount = 0;
  static unsigned long tapStartTime = 0;
  
  // Clear screen completely to prevent overlap
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  // Title
  u8g2.drawStr(10, 12, "TEST RESULTS");
  
  // Calculate pass count
  int passCount = 0;
  for(int i = 0; i < 8; i++) {
    if(testResults[i]) passCount++;
  }
  
  // Show overall score
  char scoreStr[20];
  sprintf(scoreStr, "Score: %d/8", passCount);
  u8g2.drawStr(10, 25, scoreStr);
  
  // Show page info
  char pageStr[20];
  sprintf(pageStr, "Page %d/3", currentPage + 1);
  u8g2.drawStr(80, 25, pageStr);
  
  // Show only 3 items per page
  int startIndex = currentPage * 3;
  int endIndex = min(startIndex + 3, 8);
  
  for(int i = startIndex; i < endIndex; i++) {
    int y = 35 + ((i - startIndex) * 10);
    
    if(i == currentMenuIndex) {
      u8g2.drawBox(0, y - 6, 128, 10);
      u8g2.setDrawColor(0);
    }
    
    char itemStr[30];
    sprintf(itemStr, "%s: %s", testNames[i], testResults[i] ? "PASS" : "FAIL");
    u8g2.drawStr(5, y, itemStr);
    
    u8g2.setDrawColor(1);
  }
  
  // Instructions at bottom
  u8g2.drawStr(5, 60, "Tap:Detail 2Tap:Next");
  
  u8g2.sendBuffer();
  
  // Handle touch input - Updated for dual touch sensor system
  bool currentTouch1 = (digitalRead(TOUCH_PIN_1) == HIGH);
  bool currentTouch2 = (digitalRead(TOUCH_PIN_2) == HIGH);
  
  if(currentTouch1 && !touchPressed) {
    touchPressed = true;
    tapCount++;
    if(tapCount == 1) {
      tapStartTime = millis();
    }
  } else if(!currentTouch1 && touchPressed) {
    touchPressed = false;
    
    // Check for single tap (within 300ms)
    if(tapCount == 1 && millis() - tapStartTime < 300) {
      // Single tap - show details
      showTestDetail(currentMenuIndex, testNames[currentMenuIndex], testResults[currentMenuIndex]);
      tapCount = 0;
    } else if(tapCount == 2 && millis() - tapStartTime < 300) {
      // Double tap - next page
      currentPage = (currentPage + 1) % 3;
      currentMenuIndex = currentPage * 3; // Reset to first item of new page
      tapCount = 0;
    } else {
      // Reset tap count if too slow
      tapCount = 0;
    }
  }
  
  // Handle Touch Sensor 2 for navigation
  if(currentTouch2 && millis() - lastTouchTime > 200) {
    lastTouchTime = millis();
    currentMenuIndex++;
    if(currentMenuIndex >= endIndex) {
      currentMenuIndex = startIndex; // Cycle within current page
    }
  }
  
  // Reset tap count if too much time has passed
  if(millis() - tapStartTime > 300) {
    tapCount = 0;
  }
}

void showTestDetail(int index, const char* itemName, bool status) {
  // Clear screen completely
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);
  
  // Title
  u8g2.drawStr(5, 15, itemName);
  
  // Status
  if(status) {
    u8g2.drawStr(5, 35, "PASSED");
  } else {
    u8g2.drawStr(5, 35, "FAILED");
  }
  
  // Details based on test
  u8g2.setFont(u8g2_font_6x10_tf);
  switch(index) {
    case 0: // OLED
      u8g2.drawStr(5, 50, "Display working");
      break;
    case 1: // Touch Sensor 1
      char touch1Str[20];
      sprintf(touch1Str, "Touches: %d", touchCount1);
      u8g2.drawStr(5, 50, touch1Str);
      break;
    case 2: // Touch Sensor 2
      char touch2Str[20];
      sprintf(touch2Str, "Touches: %d", touchCount2);
      u8g2.drawStr(5, 50, touch2Str);
      break;
    case 3: // Heartbeat
      char hbStr[25];
      sprintf(hbStr, "Readings: %d", heartbeatReadings);
      u8g2.drawStr(5, 50, hbStr);
      break;
    case 4: // LEDs
      char ledStr[20];
      sprintf(ledStr, "Blinks: %d", blinkCount);
      u8g2.drawStr(5, 50, ledStr);
      break;
    case 5: // Buzzer
      char buzzerStr[20];
      sprintf(buzzerStr, "Beeps: %d", beepCount);
      u8g2.drawStr(5, 50, buzzerStr);
      break;
    case 6: // SPIFFS
      u8g2.drawStr(5, 50, "File system OK");
      break;
    case 7: // WiFi
      u8g2.drawStr(5, 50, "Network test");
      break;
  }
  
  u8g2.drawStr(5, 64, "Tap to continue");
  
  u8g2.sendBuffer();
  
  // Wait for touch to continue
  while(digitalRead(TOUCH_PIN_1) == LOW) {
    delay(50);
  }
  delay(200);
  
  // Clear screen after detail view
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}