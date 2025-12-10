/*
 * Hardware Test (No OLED) - ESP32 Gemini Assistant
 * Tests all hardware components except OLED display
 *
 * Hardware Connections:
 * - Touch Sensor 1: GPIO4
 * - Touch Sensor 2: GPIO5
 * - Heartbeat Signal: GPIO34 (ADC)
 * - Heartbeat Power: GPIO32
 * - Main LED: GPIO25 + 220Ω
 * - LED 2: GPIO18 + 220Ω
 * - LED 3: GPIO19 + 220Ω
 * - LED 4: GPIO23 + 220Ω
 * - Buzzer: GPIO26
 * - All VCC: 3.3V
 * - All GND: GND
 */

#define TOUCH_PIN_1 4
#define TOUCH_PIN_2 5
#define HEARTBEAT_PIN 34
#define HEARTBEAT_POWER_PIN 32
#define LED_PIN 25
#define LED2_PIN 18
#define LED3_PIN 19
#define LED4_PIN 23
#define BUZZER_PIN 26

// LED pins array for easy control
int ledPins[4] = {LED_PIN, LED2_PIN, LED3_PIN, LED4_PIN};

// Test states
enum TestState {
  TEST_IDLE,
  TEST_TOUCH_SENSORS,
  TEST_LEDS,
  TEST_BUZZER,
  TEST_HEARTBEAT_SENSOR,
  TEST_ALL
};

TestState currentTest = TEST_IDLE;
unsigned long lastTestChange = 0;
unsigned long testInterval = 3000; // 3 seconds per test

// Touch sensor variables
bool touch1Pressed = false;
bool touch2Pressed = false;
unsigned long lastTouch1Time = 0;
unsigned long lastTouch2Time = 0;

// Heartbeat variables
int heartbeatBuffer[10];
int bufferIndex = 0;
unsigned long lastHeartbeatCheck = 0;

// LED test variables
int currentLED = 0;
bool ledDirection = true; // true = forward, false = backward

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 Hardware Test (No OLED) ===");
  Serial.println("Testing all components except OLED display");
  Serial.println();

  // Initialize pins
  pinMode(TOUCH_PIN_1, INPUT);
  pinMode(TOUCH_PIN_2, INPUT);
  pinMode(HEARTBEAT_PIN, INPUT);
  pinMode(HEARTBEAT_POWER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize LED pins
  for(int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Start with heartbeat sensor OFF
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);

  // Initialize heartbeat buffer
  for(int i = 0; i < 10; i++) {
    heartbeatBuffer[i] = 0;
  }

  Serial.println("Hardware initialized!");
  Serial.println("Available tests:");
  Serial.println("1. Touch Sensors (GPIO4, GPIO5)");
  Serial.println("2. LEDs (GPIO25, 18, 19, 23)");
  Serial.println("3. Buzzer (GPIO26)");
  Serial.println("4. Heartbeat Sensor (GPIO34 signal, GPIO32 power)");
  Serial.println("5. All tests cycle automatically");
  Serial.println();
  Serial.println("Commands:");
  Serial.println("'touch' - Test touch sensors");
  Serial.println("'leds' - Test LEDs");
  Serial.println("'buzzer' - Test buzzer");
  Serial.println("'heartbeat' - Test heartbeat sensor");
  Serial.println("'all' - Cycle through all tests");
  Serial.println("'stop' - Stop current test");
  Serial.println();

  // Start with idle state
  currentTest = TEST_IDLE;
  Serial.println("Ready! Type a command to begin testing.");
}

void loop() {
  // Handle serial commands
  if(Serial.available()) {
    String command = Serial.readString();
    command.trim();
    command.toLowerCase();

    if(command == "touch") {
      startTouchTest();
    }
    else if(command == "leds") {
      startLEDTest();
    }
    else if(command == "buzzer") {
      startBuzzerTest();
    }
    else if(command == "heartbeat") {
      startHeartbeatTest();
    }
    else if(command == "all") {
      startAllTests();
    }
    else if(command == "stop") {
      stopCurrentTest();
    }
    else {
      Serial.println("Unknown command. Use: touch, leds, buzzer, heartbeat, all, stop");
    }
  }

  // Run current test
  switch(currentTest) {
    case TEST_TOUCH_SENSORS:
      runTouchTest();
      break;
    case TEST_LEDS:
      runLEDTest();
      break;
    case TEST_BUZZER:
      runBuzzerTest();
      break;
    case TEST_HEARTBEAT_SENSOR:
      runHeartbeatTest();
      break;
    case TEST_ALL:
      runAllTests();
      break;
    default:
      // Idle state
      break;
  }

  delay(50); // Small delay for stability
}

void startTouchTest() {
  Serial.println("\n=== Touch Sensor Test ===");
  Serial.println("Touch Sensor 1: GPIO4");
  Serial.println("Touch Sensor 2: GPIO5");
  Serial.println("Touch the sensors and watch the serial output");
  Serial.println("Type 'stop' to end test");
  currentTest = TEST_TOUCH_SENSORS;
}

void runTouchTest() {
  bool currentTouch1 = digitalRead(TOUCH_PIN_1);
  bool currentTouch2 = digitalRead(TOUCH_PIN_2);
  unsigned long currentTime = millis();

  // Touch Sensor 1
  if(currentTouch1 && !touch1Pressed) {
    if(currentTime - lastTouch1Time > 100) { // Debounce
      touch1Pressed = true;
      lastTouch1Time = currentTime;
      Serial.println("Touch Sensor 1 (GPIO4) - PRESSED");
      // Quick LED feedback
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
    }
  }
  else if(!currentTouch1 && touch1Pressed) {
    touch1Pressed = false;
    Serial.println("Touch Sensor 1 (GPIO4) - RELEASED");
  }

  // Touch Sensor 2
  if(currentTouch2 && !touch2Pressed) {
    if(currentTime - lastTouch2Time > 100) { // Debounce
      touch2Pressed = true;
      lastTouch2Time = currentTime;
      Serial.println("Touch Sensor 2 (GPIO5) - PRESSED");
      // Quick LED feedback
      digitalWrite(LED2_PIN, HIGH);
      delay(100);
      digitalWrite(LED2_PIN, LOW);
    }
  }
  else if(!currentTouch2 && touch2Pressed) {
    touch2Pressed = false;
    Serial.println("Touch Sensor 2 (GPIO5) - RELEASED");
  }
}

void startLEDTest() {
  Serial.println("\n=== LED Test ===");
  Serial.println("Testing LEDs: GPIO25, GPIO18, GPIO19, GPIO23");
  Serial.println("Watch the LEDs light up in sequence");
  Serial.println("Type 'stop' to end test");
  currentTest = TEST_LEDS;
  currentLED = 0;
  ledDirection = true;
  lastTestChange = millis();
}

void runLEDTest() {
  unsigned long currentTime = millis();

  if(currentTime - lastTestChange > 300) { // Change LED every 300ms
    // Turn off all LEDs
    for(int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    // Turn on current LED
    digitalWrite(ledPins[currentLED], HIGH);

    Serial.print("LED ");
    Serial.print(currentLED + 1);
    Serial.print(" (GPIO");
    Serial.print(ledPins[currentLED]);
    Serial.println(") - ON");

    // Move to next LED
    if(ledDirection) {
      currentLED++;
      if(currentLED >= 4) {
        currentLED = 2; // Go backwards from LED 4
        ledDirection = false;
      }
    } else {
      currentLED--;
      if(currentLED < 0) {
        currentLED = 1; // Go forwards from LED 1
        ledDirection = true;
      }
    }

    lastTestChange = currentTime;
  }
}

void startBuzzerTest() {
  Serial.println("\n=== Buzzer Test ===");
  Serial.println("Testing Buzzer: GPIO26");
  Serial.println("Listen for different tones");
  Serial.println("Type 'stop' to end test");
  currentTest = TEST_BUZZER;
  lastTestChange = millis();
}

void runBuzzerTest() {
  unsigned long currentTime = millis();

  if(currentTime - lastTestChange > 1000) { // Change tone every 1 second
    static int toneIndex = 0;
    int frequencies[] = {1000, 1500, 2000, 800};
    int durations[] = {200, 300, 400, 500};

    tone(BUZZER_PIN, frequencies[toneIndex], durations[toneIndex]);

    Serial.print("Buzzer tone: ");
    Serial.print(frequencies[toneIndex]);
    Serial.print("Hz for ");
    Serial.print(durations[toneIndex]);
    Serial.println("ms");

    toneIndex = (toneIndex + 1) % 4;
    lastTestChange = currentTime;
  }
}

void startHeartbeatTest() {
  Serial.println("\n=== Heartbeat Sensor Test ===");
  Serial.println("Heartbeat Signal: GPIO34 (ADC)");
  Serial.println("Heartbeat Power: GPIO32");
  Serial.println("Place finger on sensor when prompted");
  Serial.println("Type 'stop' to end test");

  // Turn on heartbeat sensor power
  digitalWrite(HEARTBEAT_POWER_PIN, HIGH);
  Serial.println("Heartbeat sensor power ON");

  delay(1000); // Wait for sensor to stabilize

  Serial.println("Place your finger on the heartbeat sensor now!");
  Serial.println("Watch for ADC readings and BPM calculations");

  currentTest = TEST_HEARTBEAT_SENSOR;
  lastHeartbeatCheck = millis();
}

void runHeartbeatTest() {
  unsigned long currentTime = millis();

  if(currentTime - lastHeartbeatCheck > 100) { // Check every 100ms
    int sensorValue = analogRead(HEARTBEAT_PIN);
    heartbeatBuffer[bufferIndex] = sensorValue;
    bufferIndex = (bufferIndex + 1) % 10;

    Serial.print("ADC Value: ");
    Serial.print(sensorValue);
    Serial.print(" | Voltage: ");
    Serial.print((sensorValue * 3.3) / 4095.0, 3);
    Serial.println("V");

    lastHeartbeatCheck = currentTime;
  }
}

void startAllTests() {
  Serial.println("\n=== All Tests Cycle ===");
  Serial.println("Cycling through all hardware tests automatically");
  Serial.println("Type 'stop' to end cycling");
  currentTest = TEST_ALL;
  lastTestChange = millis();
}

void runAllTests() {
  unsigned long currentTime = millis();

  if(currentTime - lastTestChange > testInterval) {
    // Stop current test
    stopCurrentTest();

    // Move to next test
    static int testIndex = 0;
    TestState nextTests[] = {TEST_TOUCH_SENSORS, TEST_LEDS, TEST_BUZZER, TEST_HEARTBEAT_SENSOR};

    currentTest = nextTests[testIndex];
    testIndex = (testIndex + 1) % 4;

    // Start the new test
    switch(currentTest) {
      case TEST_TOUCH_SENSORS:
        startTouchTest();
        break;
      case TEST_LEDS:
        startLEDTest();
        break;
      case TEST_BUZZER:
        startBuzzerTest();
        break;
      case TEST_HEARTBEAT_SENSOR:
        startHeartbeatTest();
        break;
    }

    lastTestChange = currentTime;
  }

  // Run the current test
  switch(currentTest) {
    case TEST_TOUCH_SENSORS:
      runTouchTest();
      break;
    case TEST_LEDS:
      runLEDTest();
      break;
    case TEST_BUZZER:
      runBuzzerTest();
      break;
    case TEST_HEARTBEAT_SENSOR:
      runHeartbeatTest();
      break;
  }
}

void stopCurrentTest() {
  // Turn off all outputs
  for(int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  noTone(BUZZER_PIN);
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);

  if(currentTest != TEST_IDLE) {
    Serial.println("Test stopped");
  }

  currentTest = TEST_IDLE;
}
