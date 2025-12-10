/*
 * GPIO Hardware Test for Heartbeat Sensor
 * Simple test to verify GPIO 32 and GPIO 34 connections
 * 
 * Hardware:
 * - GPIO 32: Heartbeat sensor power control
 * - GPIO 34: Heartbeat sensor analog input
 * - GPIO 25: Built-in LED for visual feedback
 */

#define HEARTBEAT_POWER_PIN 32
#define HEARTBEAT_PIN 34
#define LED_PIN 25

void setup() {
  Serial.begin(115200);
  Serial.println("=== GPIO Hardware Test ===");
  
  // Initialize pins
  pinMode(HEARTBEAT_POWER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(HEARTBEAT_PIN, INPUT);
  
  Serial.println("GPIO Test initialized");
  Serial.println("GPIO 32: Heartbeat sensor power control");
  Serial.println("GPIO 34: Heartbeat sensor analog input");
  Serial.println("GPIO 25: Built-in LED");
  
  // Test GPIO 32 (Power control)
  Serial.println("\nTesting GPIO 32 (Power control)...");
  digitalWrite(HEARTBEAT_POWER_PIN, HIGH);
  Serial.println("GPIO 32 set to HIGH - Heartbeat sensor should be powered");
  delay(2000);
  
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  Serial.println("GPIO 32 set to LOW - Heartbeat sensor should be off");
  delay(2000);
  
  // Test GPIO 34 (Analog input)
  Serial.println("\nTesting GPIO 34 (Analog input)...");
  Serial.println("Reading analog values (should change when sensor is powered):");
  
  for(int i = 0; i < 10; i++) {
    int rawValue = analogRead(HEARTBEAT_PIN);
    float voltage = (rawValue * 3.3) / 4095.0;
    
    Serial.print("Reading ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(rawValue);
    Serial.print(" (");
    Serial.print(voltage);
    Serial.println("V)");
    
    delay(500);
  }
  
  // Test LED
  Serial.println("\nTesting GPIO 25 (LED)...");
  for(int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(500);
    
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(500);
  }
  
  Serial.println("\n=== Hardware Test Complete ===");
  Serial.println("If you see changing analog values when sensor is powered,");
  Serial.println("the connections are working correctly!");
}

void loop() {
  // Continuous monitoring
  static unsigned long lastCheck = 0;
  
  if(millis() - lastCheck > 2000) { // Check every 2 seconds
    lastCheck = millis();
    
    // Power on sensor
    digitalWrite(HEARTBEAT_POWER_PIN, HIGH);
    delay(100); // Small delay for sensor to stabilize
    
    int rawValue = analogRead(HEARTBEAT_PIN);
    float voltage = (rawValue * 3.3) / 4095.0;
    
    Serial.print("Sensor ON - Raw: ");
    Serial.print(rawValue);
    Serial.print(" (");
    Serial.print(voltage);
    Serial.print("V)");
    
    // Power off sensor
    digitalWrite(HEARTBEAT_POWER_PIN, LOW);
    delay(100);
    
    int rawValueOff = analogRead(HEARTBEAT_PIN);
    float voltageOff = (rawValueOff * 3.3) / 4095.0;
    
    Serial.print(" | Sensor OFF - Raw: ");
    Serial.print(rawValueOff);
    Serial.print(" (");
    Serial.print(voltageOff);
    Serial.println("V)");
    
    // Check if there's a difference (sensor is working)
    if(abs(rawValue - rawValueOff) > 50) {
      Serial.println("✓ Heartbeat sensor is responding!");
    } else {
      Serial.println("⚠ Heartbeat sensor may not be connected properly");
    }
  }
}
