/*
 * ESP32 Gemini Assistant
 * Interactive OLED-based assistant with heartbeat monitoring and AI integration
 * 
 * Hardware:
 * - OLED Display (I2C): SDA=GPIO21, SCL=GPIO22
 * - Touch Sensor 1 (TTP223): OUT=GPIO4 (Open menu and navigate)
 * - Touch Sensor 2 (TTP223): OUT=GPIO5 (Move down list, cycle to top)
 * - Heartbeat Sensor: Pulse=GPIO34 (ADC)
 * - LED Indicator: GPIO25 (Main LED)
 * - Additional LEDs: GPIO18, GPIO19, GPIO23
 * - Passive Buzzer: GPIO26
 * 
 * Features:
 * - Dual touch sensor navigation (menu open/select and list navigation)
 * - Heartbeat monitoring with BPM calculation
 * - Task management with notifications
 * - Gemini AI integration via web interface
 * - Animated OLED display with various states
 */

#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <sys/time.h>
#include <ArduinoOTA.h>
#include "../config.h"
#include "../animations.h"

// Global Objects
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
WebServer server(WEB_SERVER_PORT);

// Touch Detection Variables - Two sensors for menu navigation
unsigned long lastTouchTime1 = 0;
unsigned long lastTouchTime2 = 0;
int touchCount1 = 0;
int touchCount2 = 0;
unsigned long firstTouchTime1 = 0;
unsigned long firstTouchTime2 = 0;
bool touchPressed1 = false;
bool touchPressed2 = false;

// Menu Navigation Variables
bool menuOpen = false;
int currentMenuItem = 0;
int maxMenuItems = 6; // Number of menu options (OLED): System, Settings, Gemini, Tasks, Heartbeat, Lamp
String menuItems[] = { "Gemini Chat", "Tasks", "Heartbeat", "Lamp","System", "Settings",};
unsigned long lastMenuUpdate = 0;
unsigned long menuOpenTime = 0; // Track when menu was opened for timeout

// Heartbeat Sensor Variables
int heartbeatBuffer[HEARTBEAT_BUFFER_SIZE];
int bufferIndex = 0;
unsigned long lastPeakTime = 0;
int currentBPM = 0;
bool ledState = false;
bool heartbeatSensorActive = false; // Only active when in menu mode

// User Settings
String userName = "User";
String welcomeMessage = "";

// Animation Variables
unsigned long lastAnimationTime = 0;
int currentFrame = 0;
int animationState = 0; // 0=idle, 1=blink, 2=happy, 3=heart, 4=look_left, 5=look_right, 6=sad, 7=angry, 8=cute, 9=wonder
unsigned long lastIdleBlink = 0;
bool happy_state = false;
int skip = 1; // Animation speed multiplier
int q = 0; // Animation variable

// Task Management Variables
struct Task {
  String title;
  String description;
  int hour;
  int minute;
  bool completed;
  bool due;
};

Task tasks[MAX_TASKS];
int taskCount = 0;
bool hasDueTask = false;

// System State
SystemState currentState = STATE_IDLE;
String currentNotification = "";

// LED Control Variables (4 LEDs)
int ledPins[4] = {LED_PIN, 18, 19, 23}; // Main LED + 3 additional LEDs (avoiding GPIO 26 for buzzer)
int ledStates[4] = {LOW, LOW, LOW, LOW};

// Lamp Control Variables
bool lampMode = false;
int lampPattern = 0; // 0=off, 1=all on, 2=wave, 3=chase, 4=breathing, 5=rainbow
int lampBrightness = 100; // 0-100%
unsigned long lastLampUpdate = 0;
int lampStep = 0;

// Menu Navigation Variables
int currentMenuOption = 0; // 0=Tasks, 1=Heartbeat, 2=Gemini, 3=Lamp, 4=Alarm
const int totalMenuOptions = 5;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  
  // Initialize hardware pins
  pinMode(TOUCH_PIN_1, INPUT);
  pinMode(TOUCH_PIN_2, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(HEARTBEAT_PIN, INPUT);
  pinMode(HEARTBEAT_POWER_PIN, OUTPUT);  // Control heartbeat sensor power
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize additional LED pins
  for(int i = 1; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  // Start with heartbeat sensor OFF
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  
  // Initialize display
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  u8g2.begin();
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  Serial.println("SPIFFS initialized successfully!");
  
  // Initialize heartbeat buffer
  for(int i = 0; i < HEARTBEAT_BUFFER_SIZE; i++) {
    heartbeatBuffer[i] = 0;
  }
  
  // Load WiFi credentials from SPIFFS if available
  String wifiSSID = WIFI_SSID;
  String wifiPassword = WIFI_PASSWORD;
  
  if(SPIFFS.exists("/wifi_config.json")) {
    File file = SPIFFS.open("/wifi_config.json", "r");
    if(file) {
      String jsonString = file.readString();
      file.close();
      
      DynamicJsonDocument doc(512);
      deserializeJson(doc, jsonString);
      
      wifiSSID = doc["ssid"].as<String>();
      wifiPassword = doc["password"].as<String>();
      
      Serial.println("Loaded WiFi credentials from SPIFFS");
    }
  }
  
  // Check if we have saved WiFi credentials
  if(wifiSSID.length() > 0 && wifiPassword.length() > 0) {
    // Try to connect to saved WiFi first
    Serial.println("Attempting to connect to saved WiFi: " + wifiSSID);
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
    
    int wifiRetryCount = 0;
    const int maxRetries = 3;
    
    while (WiFi.status() != WL_CONNECTED && wifiRetryCount < maxRetries) {
      delay(2000);
      wifiRetryCount++;
      Serial.println("WiFi connection attempt " + String(wifiRetryCount) + "/" + String(maxRetries));
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected successfully!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
        break;
      }
    }
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Failed to connect to saved WiFi, starting AP mode...");
      WiFi.disconnect();
      delay(1000);
      WiFi.mode(WIFI_AP);
      WiFi.softAP("Mini Gem v1", "gemini123");
      Serial.println("Access Point started!");
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
    }
  } else {
    // No saved credentials, start in Access Point mode
    Serial.println("No saved WiFi credentials, starting in Access Point mode...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Mini Gem v1", "gemini123");
    Serial.println("Access Point started!");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("Connect to: Mini Gem v1");
    Serial.println("Password: gemini123");
    
    // Show AP mode on display
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 15, "AP Mode");
    u8g2.drawStr(5, 30, "Mini Gem v1");
    u8g2.drawStr(5, 45, "Password: gemini123");
    u8g2.sendBuffer();
  }
  
  // Configure time
  configTime(0, 0, "pool.ntp.org");
  
  // Load tasks from SPIFFS
  loadTasks();
  loadUserName();
  
  // Setup web server routes
  setupWebServer();
  server.begin();
  
  // Setup OTA (Over-The-Air) updates
  setupOTA();
  
  // Initial display with welcome message
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5, 15, "Welcome!");
  u8g2.drawStr(5, 30, userName.c_str());
  u8g2.drawStr(5, 45, "Gemini Ready");
  u8g2.sendBuffer();
  
  // Set welcome message for display
  welcomeMessage = getGreetingMessage();
  
  // Show welcome message with sound
  showWelcomeMessage();
  
  Serial.println("ESP32 Gemini Assistant initialized!");
}

// OTA Setup Function
void setupOTA() {
  ArduinoOTA.setHostname("Mini-Gem-v1");
  ArduinoOTA.setPassword("gemini123"); // Change this password
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("Start updating " + type);
    
    // Show update message on display
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 15, "OTA Update");
    u8g2.drawStr(5, 30, "Starting...");
    u8g2.sendBuffer();
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 15, "OTA Update");
    u8g2.drawStr(5, 30, "Complete!");
    u8g2.sendBuffer();
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    
    // Show progress on display
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 15, "OTA Update");
    char progressStr[20];
    sprintf(progressStr, "Progress: %u%%", (progress / (total / 100)));
    u8g2.drawStr(5, 30, progressStr);
    u8g2.sendBuffer();
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    
    // Show error on display
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 15, "OTA Error");
    u8g2.drawStr(5, 30, "Update Failed");
    u8g2.sendBuffer();
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void loop() {
  // Handle OTA updates
  ArduinoOTA.handle();
  
  // Handle web server requests
  server.handleClient();
  
  // Handle touch input for both sensors
  handleTouchInput1(); // Touch Sensor 1: Open menu and navigate
  handleTouchInput2(); // Touch Sensor 2: Move down list, cycle to top
  
  // Check for menu timeout (5 seconds)
  checkMenuTimeout();
  
  // Menu selection is now handled in handleTouchInput()
  
  // Update heartbeat monitoring
  updateHeartbeatSensor();
  
  // Update animations
  updateAnimations();
  
  // Update lamp patterns
  updateLampPatterns();
  
  // Handle lamp touch input
  if(currentState == STATE_LAMP_CONTROL) {
    handleLampTouch();
  }
  
  // Handle alarm touch input
  if(currentState == STATE_ALARM_CONTROL) {
    handleAlarmTouch();
  }
  
  // Handle settings touch input
  if(currentState == STATE_SETTINGS) {
    handleSettingsTouch();
  }
  
  // Check for due tasks
  checkDueTasks();
  
  // Update display based on current state (only when needed)
  static unsigned long lastDisplayUpdate = 0;
  unsigned long currentTime = millis();
  
  // Update display every 100ms to prevent flickering
  if(currentTime - lastDisplayUpdate > 100) {
  updateDisplay();
    lastDisplayUpdate = currentTime;
  }
  
  delay(MAIN_LOOP_DELAY_MS);
}

// Touch Input Handling
// Touch Input Handling - Sensor 1 (Open menu and navigate)
void handleTouchInput1() {
  bool currentTouch = digitalRead(TOUCH_PIN_1);
    unsigned long currentTime = millis();
    
  // Detect touch press (HIGH to LOW transition with debouncing)
  if(currentTouch && !touchPressed1) {
    // Add debounce delay - only trigger if touch is stable for 50ms
    if(currentTime - lastTouchTime1 > 50) {
      touchPressed1 = true;
      firstTouchTime1 = currentTime;
      touchCount1 = 1;
      lastTouchTime1 = currentTime;
      
      // Open menu or navigate
      if(!menuOpen) {
        menuOpen = true;
        currentMenuItem = 0;
        menuOpenTime = currentTime; // Record when menu was opened
        Serial.println("Menu opened");
        
        // LED and buzzer feedback for opening menu
        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, 1000, 200); // Higher pitch for menu open
        delay(200);
        digitalWrite(LED_PIN, LOW);
      } else {
        // Navigate to selected menu item
        selectMenuItem();
        
        // LED and buzzer feedback for selection
        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, 1500, 300); // Higher pitch for selection
        delay(300);
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
  
  // Detect touch release
  if(!currentTouch && touchPressed1) {
    touchPressed1 = false;
    lastTouchTime1 = currentTime; // Update last touch time on release
  }
}

// Touch Input Handling - Sensor 2 (Move down list, cycle to top)
void handleTouchInput2() {
  bool currentTouch = digitalRead(TOUCH_PIN_2);
  unsigned long currentTime = millis();
  
  // Detect touch press (HIGH to LOW transition with debouncing)
  if(currentTouch && !touchPressed2) {
    // Add debounce delay - only trigger if touch is stable for 50ms
    if(currentTime - lastTouchTime2 > 50) {
      touchPressed2 = true;
      firstTouchTime2 = currentTime;
      touchCount2 = 1;
      lastTouchTime2 = currentTime;
      
      if(menuOpen) {
        // Move to next menu item
        currentMenuItem++;
        if(currentMenuItem >= maxMenuItems) {
          currentMenuItem = 0; // Cycle back to top
        }
        menuOpenTime = currentTime; // Reset timeout when navigating
        Serial.println("Menu item: " + String(currentMenuItem) + " - " + menuItems[currentMenuItem]);
        
        // LED and buzzer feedback for navigation
        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, 800, 150); // Lower pitch for navigation
        delay(150);
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
  
  // Detect touch release
  if(!currentTouch && touchPressed2) {
    touchPressed2 = false;
    lastTouchTime2 = currentTime; // Update last touch time on release
  }
}

// Check for menu timeout - return to idle screen after 5 seconds
void checkMenuTimeout() {
  if(menuOpen) {
    unsigned long currentTime = millis();
    if(currentTime - menuOpenTime > 5000) { // 5 seconds timeout
      menuOpen = false;
      currentState = STATE_IDLE;
      Serial.println("Menu timeout - returning to idle screen");
      
      // Turn off heartbeat sensor power when returning to idle
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);
    }
  }
}

// Select current menu item
void selectMenuItem() {
  Serial.println("Selected: " + menuItems[currentMenuItem]);
  menuOpen = false; // Close menu after selection
  
  // Handle different menu selections with LED feedback
  switch(currentMenuItem) {
    case 0: // Gemini Chat
      Serial.println("Opening Gemini Chat...");
      currentState = STATE_GEMINI_CHAT;
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);
      // Gemini Chat - Double LED blink
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      break;
    case 1: // Tasks
      Serial.println("Opening Tasks...");
      currentState = STATE_TASK_DISPLAY;
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);
      // Tasks - Triple LED blink
      for(int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(80);
        digitalWrite(LED_PIN, LOW);
        delay(80);
      }
      break;
    case 2: // Heartbeat
      Serial.println("Opening Heartbeat Monitor...");
      currentState = STATE_HEARTBEAT_MONITOR;
      heartbeatSensorActive = true;
      digitalWrite(HEARTBEAT_POWER_PIN, HIGH);  // Turn on heartbeat sensor power
      // Heartbeat - All LEDs blink together
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(18, HIGH);
      digitalWrite(19, HIGH);
      digitalWrite(23, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(18, LOW);
      digitalWrite(19, LOW);
      digitalWrite(23, LOW);
      break;
    case 3: // Lamp
      Serial.println("Opening Lamp Control...");
      currentState = STATE_LAMP_CONTROL;
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);
      // Lamp - LED 4 blink
      digitalWrite(23, HIGH);
      delay(150);
      digitalWrite(23, LOW);
      break;
    case 4: // System
      Serial.println("Opening System...");
      currentState = STATE_ALARM_CONTROL; // repurposed to drawSystemScreen
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);
      // System - LED 3 blink
      digitalWrite(19, HIGH);
      delay(150);
      digitalWrite(19, LOW);
      break;
    case 5: // Settings
      Serial.println("Opening Settings...");
      currentState = STATE_SETTINGS; // New state for settings
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);
      // Settings - LED 2 blink
      digitalWrite(18, HIGH);
      delay(150);
      digitalWrite(18, LOW);
      break;
  }
}

// Old processTouchSequence function removed - now using dual touch sensor system

void handleMenuSelection() {
  static unsigned long lastMenuTouch = 0;
  static bool menuTouchPressed = false;
  static int menuTapCount = 0;
  static unsigned long menuTapStart = 0;
  
  bool currentTouch = (digitalRead(TOUCH_PIN_1) == HIGH);
  
  if(currentTouch && !menuTouchPressed) {
    menuTouchPressed = true;
    menuTapCount++;
    if(menuTapCount == 1) {
      menuTapStart = millis();
    }
  } else if(!currentTouch && menuTouchPressed) {
    menuTouchPressed = false;
    
    // Check for single tap (within 300ms)
    if(menuTapCount == 1 && millis() - menuTapStart < 300) {
      // Single tap - Select current option
      selectMenuOption();
      menuTapCount = 0;
    } else if(menuTapCount == 2 && millis() - menuTapStart < 300) {
      // Double tap - Move to next option
      currentMenuOption = (currentMenuOption + 1) % totalMenuOptions;
      menuTapCount = 0;
    } else {
      // Reset tap count if too slow
      menuTapCount = 0;
    }
  }
  
  // Reset tap count if too much time has passed
  if(millis() - menuTapStart > 300) {
    menuTapCount = 0;
  }
}

void selectMenuOption() {
  switch(currentMenuOption) {
    case 0: // Tasks
      currentState = STATE_TASK_DISPLAY;
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Turn OFF heartbeat sensor power
      lampMode = false;
      break;
    case 1: // Heartbeat
      currentState = STATE_HEARTBEAT_MONITOR;
      heartbeatSensorActive = true;
      digitalWrite(HEARTBEAT_POWER_PIN, HIGH); // Turn ON heartbeat sensor power
      lampMode = false;
      break;
    case 2: // Gemini
      currentState = STATE_GEMINI_CHAT;
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Turn OFF heartbeat sensor power
      lampMode = false;
      break;
    case 3: // Lamp
      currentState = STATE_LAMP_CONTROL;
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Turn OFF heartbeat sensor power
      lampMode = true;
      break;
    case 4: // Alarm
      currentState = STATE_ALARM_CONTROL;
      heartbeatSensorActive = false;
      digitalWrite(HEARTBEAT_POWER_PIN, LOW);  // Turn OFF heartbeat sensor power
      lampMode = false;
      break;
  }
  
  // Play selection sound
  playBuzzerSound(BUZZER_FREQUENCY_HIGH, BUZZER_DURATION_SHORT);
}

// Animation Functions
void triggerWakeAnimation() {
  // Only trigger animation if in idle state
  if(currentState == STATE_IDLE) {
  animationState = ANIM_BLINK;
  currentFrame = 0;
  lastAnimationTime = millis();
    
    // Temporarily show blink animation on idle screen
    u8g2.clearBuffer();
    blink();
    
    // Show other idle screen elements
    // Show time
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, timeStr);
    
    // Show status
    const char* statusText = "Ready";
    if(WiFi.status() != WL_CONNECTED) {
      statusText = "WiFi?";
    } else if(hasDueTask) {
      statusText = "Alert!";
    } else if(currentBPM > 0) {
      statusText = "Heart";
    } else if(taskCount > 0) {
      statusText = "Tasks";
    }
    u8g2.drawStr(80, 60, statusText);
    
    u8g2.sendBuffer();
    
    // Reset to normal after animation
    delay(200);
    animationState = ANIM_IDLE;
  }
}

void showLastNotification() {
  if(hasDueTask) {
    currentState = STATE_NOTIFICATION;
    currentNotification = "Task Due!";
  } else {
    currentState = STATE_TASK_DISPLAY;
  }
}

void openMainMenu() {
  currentState = STATE_MENU;
  animationState = ANIM_IDLE;
  heartbeatSensorActive = false; // Keep heartbeat sensor OFF in menu
  digitalWrite(HEARTBEAT_POWER_PIN, LOW); // Turn OFF heartbeat sensor power
  currentMenuOption = 0; // Reset to first option
}

// Buzzer Functions
void playBuzzerSound(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

void playBuzzerMelody() {
  // Play a simple melody for triple tap
  int melody[] = {1000, 1200, 1500, 2000};
  int noteDuration = BUZZER_DURATION_SHORT;
  
  for(int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, melody[i], noteDuration);
    delay(noteDuration + 50);
  }
}

void playNotificationSound() {
  // Play notification sound for due tasks
  tone(BUZZER_PIN, BUZZER_FREQUENCY_HIGH, BUZZER_DURATION_MEDIUM);
  delay(BUZZER_DURATION_MEDIUM + 100);
  tone(BUZZER_PIN, BUZZER_FREQUENCY_LOW, BUZZER_DURATION_MEDIUM);
}

void playHeartbeatSound() {
  // Play heartbeat sound synchronized with LED
  tone(BUZZER_PIN, BUZZER_FREQUENCY_MEDIUM, BUZZER_DURATION_SHORT);
}

void playGeminiResponseSound() {
  // Play success sound for Gemini responses
  int successMelody[] = {1500, 1800, 2000};
  for(int note : successMelody) {
    tone(BUZZER_PIN, note, BUZZER_DURATION_SHORT);
    delay(BUZZER_DURATION_SHORT + 50);
  }
}

// Heartbeat Sensor Functions
void updateHeartbeatSensor() {
  // Only update heartbeat when sensor is active (in menu mode)
  if(!heartbeatSensorActive) {
    return;
  }

  // Check if sensor power is on
  if(digitalRead(HEARTBEAT_POWER_PIN) == LOW) {
    return; // Sensor is not powered
  }

  int sensorValue = analogRead(HEARTBEAT_PIN);
  heartbeatBuffer[bufferIndex] = sensorValue;
  bufferIndex = (bufferIndex + 1) % HEARTBEAT_BUFFER_SIZE;

  // Simple peak detection
  if(detectPeak()) {
    unsigned long currentTime = millis();
    if(lastPeakTime > 0) {
      int interval = currentTime - lastPeakTime;
      if(interval > MIN_PEAK_INTERVAL_MS && interval < MAX_PEAK_INTERVAL_MS) {
        currentBPM = 60000 / interval;
        currentBPM = CLAMP(currentBPM, MIN_BPM, MAX_BPM);

        // Sync all 4 LEDs with heartbeat
        ledState = !ledState;
        for(int i = 0; i < 4; i++) {
          digitalWrite(ledPins[i], ledState);
        }

        // Play heartbeat sound
        playHeartbeatSound();

        // Trigger heart animation
        if(currentState == STATE_MENU) {
          animationState = ANIM_HEART;
          currentFrame = 0;
          lastAnimationTime = millis();
        }
      }
    }
    lastPeakTime = currentTime;
  }
}

bool detectPeak() {
  int current = heartbeatBuffer[bufferIndex];
  int previous = heartbeatBuffer[(bufferIndex - 1 + HEARTBEAT_BUFFER_SIZE) % HEARTBEAT_BUFFER_SIZE];
  int next = heartbeatBuffer[(bufferIndex + 1) % HEARTBEAT_BUFFER_SIZE];
  
  return (current > previous && current > next && current > HEARTBEAT_THRESHOLD);
}

// Animation Update
void updateAnimations() {
  // Only run animations when not in idle state
  if(currentState == STATE_IDLE) {
    return; // Don't run animations on idle screen
  }
  
  unsigned long currentTime = millis();
  
  // Update animation based on current state
  switch(animationState) {
    case ANIM_IDLE:
      // Don't animate in idle state
      break;
      
    case ANIM_BLINK:
      blink();
      break;
      
    case ANIM_HAPPY:
      happy();
      break;
      
    case ANIM_HEART:
      // Heart animation for heartbeat detection
      drawHeartAnimation();
      break;
      
    case ANIM_LOOK_LEFT:
      lefteye();
      break;
      
    case ANIM_LOOK_RIGHT:
      righteye();
      break;
      
    case ANIM_SAD:
      sad();
      break;
      
    case ANIM_ANGRY:
      angry();
      break;
      
    case ANIM_CUTE:
      cute();
      break;
      
    case ANIM_WONDER:
      wonder();
      break;
      
    default:
      normal();
      break;
  }
}

// Task Management Functions
void loadTasks() {
  if(SPIFFS.exists(TASKS_FILENAME)) {
    File file = SPIFFS.open(TASKS_FILENAME, "r");
    if(file) {
      String jsonString = file.readString();
      file.close();
      
      DynamicJsonDocument doc(JSON_BUFFER_SIZE);
      deserializeJson(doc, jsonString);
      
      JsonArray taskArray = doc["tasks"];
      taskCount = MIN(taskArray.size(), MAX_TASKS);
      
      for(int i = 0; i < taskCount; i++) {
        JsonObject task = taskArray[i];
        tasks[i].title = task["title"].as<String>();
        tasks[i].description = task["description"].as<String>();
        tasks[i].hour = task["hour"];
        tasks[i].minute = task["minute"];
        tasks[i].completed = task["completed"];
        tasks[i].due = false;
      }
    }
  }
}

void saveTasks() {
  DynamicJsonDocument doc(JSON_BUFFER_SIZE);
  JsonArray taskArray = doc.createNestedArray("tasks");
  
  for(int i = 0; i < taskCount; i++) {
    JsonObject task = taskArray.createNestedObject();
    task["title"] = tasks[i].title;
    task["description"] = tasks[i].description;
    task["hour"] = tasks[i].hour;
    task["minute"] = tasks[i].minute;
    task["completed"] = tasks[i].completed;
  }
  
  File file = SPIFFS.open(TASKS_FILENAME, "w");
  if(file) {
    serializeJson(doc, file);
    file.close();
  }
}

// Username Management Functions
void loadUserName() {
  if(SPIFFS.exists("/username.txt")) {
    File file = SPIFFS.open("/username.txt", "r");
    if(file) {
      userName = file.readString();
      userName.trim(); // Remove whitespace
      file.close();
    }
  }
}

void saveUserName() {
  File file = SPIFFS.open("/username.txt", "w");
  if(file) {
    file.print(userName);
    file.close();
  }
}

String getGreetingMessage() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  int hour = timeinfo->tm_hour;
  
  String greeting;
  if(hour >= 5 && hour < 12) {
    greeting = "Good morning, " + userName;
  } else if(hour >= 12 && hour < 17) {
    greeting = "Good afternoon, " + userName;
  } else if(hour >= 17 && hour < 21) {
    greeting = "Good evening, " + userName;
  } else {
    greeting = "Good night, " + userName;
  }
  
  return greeting;
}

void showWelcomeMessage() {
  // Show welcome message on display
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  
  // Show current date and time at the top
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  char timeStr[20];
  sprintf(timeStr, "%02d:%02d %02d/%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_mday, timeinfo->tm_mon + 1);
  u8g2.drawStr(0, 10, timeStr);
  
  // Center "Welcome [username]"
  String welcomeText = "Welcome " + userName;
  int textWidth = u8g2.getStrWidth(welcomeText.c_str());
  int centerX = (128 - textWidth) / 2;
  u8g2.drawStr(centerX, 30, welcomeText.c_str());
  
  // Center "your Mini gem is ready"
  String readyText = "your Mini gem is ready";
  textWidth = u8g2.getStrWidth(readyText.c_str());
  centerX = (128 - textWidth) / 2;
  u8g2.drawStr(centerX, 50, readyText.c_str());
  
  // Show WiFi status on welcome screen
  if(WiFi.status() == WL_CONNECTED) {
    // Draw WiFi icon (top right)
    u8g2.drawCircle(120, 8, 3, U8G2_DRAW_ALL); // Outer circle
    u8g2.drawCircle(120, 8, 2, U8G2_DRAW_ALL); // Middle circle
    u8g2.drawCircle(120, 8, 1, U8G2_DRAW_ALL); // Inner circle
    u8g2.drawPixel(120, 8); // Center dot
  }
  
  u8g2.sendBuffer();
  
  // Play welcome sound
  tone(BUZZER_PIN, 800, 200);
  delay(250);
  tone(BUZZER_PIN, 1000, 200);
  delay(250);
  tone(BUZZER_PIN, 1200, 300);
  
  // No eyes during welcome - just text display
  delay(20000); // Show welcome message for 20 seconds
}

void checkDueTasks() {
  static unsigned long lastCheck = 0;
  unsigned long currentTime = millis();
  
  if(currentTime - lastCheck > TASK_CHECK_INTERVAL_MS) {
    lastCheck = currentTime;
    
    hasDueTask = false;
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    for(int i = 0; i < taskCount; i++) {
      if(!tasks[i].completed) {
        if(tasks[i].hour == timeinfo->tm_hour && tasks[i].minute == timeinfo->tm_min) {
          tasks[i].due = true;
          hasDueTask = true;
          // Play notification sound for due task
          playNotificationSound();
        }
      }
    }
  }
}

// Web Server Setup
void setupWebServer() {
  // Serve main page from SPIFFS
  server.on("/", HTTP_GET, []() {
    Serial.println("Serving main page from SPIFFS...");
    if (SPIFFS.exists("/web_interface.html")) {
      Serial.println("Found web_interface.html in SPIFFS, serving from file");
      File f = SPIFFS.open("/web_interface.html", "r");
      if (f) {
        server.setContentLength(CONTENT_LENGTH_UNKNOWN);
        server.send(200, "text/html", "");
        const size_t CHUNK = 1024;
        static char buf[CHUNK+1];
        while (f.available()) {
          size_t n = f.readBytes(buf, CHUNK);
          if (n > 0) {
            buf[n] = '\0';
            server.sendContent(buf);
            yield();
          }
        }
        server.sendContent("");
        f.close();
        Serial.println("SPIFFS file served successfully");
        return;
      }
    }
    Serial.println("SPIFFS file not found, serving fallback HTML");
    // Fallback to embedded HTML if SPIFFS file not found
    String html = getMainPageHTML();
    server.send(200, "text/html", html);
  });
  
  // API endpoints
  server.on("/api/tasks", HTTP_GET, handleGetTasks);
  server.on("/api/tasks", HTTP_POST, handleCreateTask);
  server.on("/api/tasks", HTTP_PUT, handleUpdateTask);
  server.on("/api/tasks", HTTP_DELETE, handleDeleteTask);
  server.on("/api/gemini", HTTP_POST, handleGeminiRequest);
  server.on("/api/heartbeat", HTTP_GET, handleGetHeartbeat);
  server.on("/api/led", HTTP_POST, handleLEDControl);
  server.on("/api/alarm", HTTP_POST, handleAlarm);
  server.on("/api/restart", HTTP_POST, handleRestart);
  server.on("/api/wifi", HTTP_POST, handleWiFiUpdate);
  server.on("/api/wifi/scan", HTTP_GET, handleWiFiScan);
  server.on("/api/factory-reset", HTTP_POST, handleFactoryReset);
  server.on("/api/username", HTTP_POST, handleUserNameUpdate);
  server.on("/api/time", HTTP_POST, handleTimeSync);
  server.on("/api/time/ntp", HTTP_POST, handleNTPTimeSync);
}

String getMainPageHTML() {
  String html = "<!DOCTYPE html>"
         "<html>"
         "<head>"
         "    <title>ESP32 Gemini Assistant</title>"
         "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
         "    <style>"
         "        * { margin: 0; padding: 0; box-sizing: border-box; }"
         "        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; }"
         "        .container { max-width: 800px; margin: 0 auto; padding: 20px; }"
         "        .header { text-align: center; color: white; margin-bottom: 30px; }"
         "        .header h1 { font-size: 2.5em; margin-bottom: 10px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }"
         "        .header p { font-size: 1.1em; opacity: 0.9; }"
         "        .tabs { display: flex; background: rgba(255,255,255,0.1); border-radius: 10px 10px 0 0; backdrop-filter: blur(10px); }"
         "        .tab { flex: 1; padding: 15px; text-align: center; color: white; cursor: pointer; transition: all 0.3s; border-radius: 10px 10px 0 0; }"
         "        .tab:hover { background: rgba(255,255,255,0.2); }"
         "        .tab.active { background: white; color: #667eea; font-weight: bold; }"
         "        .content { background: white; border-radius: 0 0 10px 10px; padding: 30px; box-shadow: 0 10px 30px rgba(0,0,0,0.2); }"
         "        .tab-content { display: none; }"
         "        .tab-content.active { display: block; }"
         "        .section { margin-bottom: 25px; }"
         "        .section h3 { color: #333; margin-bottom: 15px; font-size: 1.3em; border-bottom: 2px solid #667eea; padding-bottom: 5px; }"
         "        .form-group { margin-bottom: 15px; }"
         "        .form-group label { display: block; margin-bottom: 5px; color: #555; font-weight: 500; }"
         "        input, textarea, button, select { width: 100%; padding: 12px; border: 2px solid #e1e5e9; border-radius: 8px; font-size: 14px; transition: all 0.3s; }"
         "        input:focus, textarea:focus, select:focus { outline: none; border-color: #667eea; box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1); }"
         "        button { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; cursor: pointer; font-weight: 600; text-transform: uppercase; letter-spacing: 1px; }"
         "        button:hover { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4); }"
         "        button:active { transform: translateY(0); }"
         "        .btn-secondary { background: linear-gradient(135deg, #6c757d 0%, #495057 100%); }"
         "        .btn-danger { background: linear-gradient(135deg, #dc3545 0%, #c82333 100%); }"
         "        .btn-success { background: linear-gradient(135deg, #28a745 0%, #20c997 100%); }"
         "        .led-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; margin-bottom: 15px; }"
         "        .led-btn { padding: 15px; font-size: 16px; }"
         "        .heartbeat-display { text-align: center; padding: 20px; background: linear-gradient(135deg, #ff6b6b 0%, #ee5a52 100%); color: white; border-radius: 10px; margin-bottom: 15px; }"
         "        .heartbeat-display h2 { font-size: 2em; margin-bottom: 10px; }"
         "        .heartbeat-display .bpm { font-size: 3em; font-weight: bold; }"
         "        .task-item { background: #f8f9fa; padding: 15px; margin: 10px 0; border-radius: 8px; border-left: 4px solid #667eea; }"
         "        .task-item h4 { color: #333; margin-bottom: 5px; }"
         "        .task-item p { color: #666; margin-bottom: 5px; }"
         "        .task-item .time { color: #667eea; font-weight: bold; }"
         "        .status-indicator { display: inline-block; width: 10px; height: 10px; border-radius: 50%; margin-right: 8px; }"
         "        .status-online { background: #28a745; }"
         "        .status-offline { background: #dc3545; }"
         "        .alarm-item { background: #fff3cd; border: 1px solid #ffeaa7; padding: 15px; margin: 10px 0; border-radius: 8px; }"
         "        .system-info { background: #e9ecef; padding: 15px; border-radius: 8px; margin-bottom: 15px; }"
         "        .system-info h4 { color: #495057; margin-bottom: 10px; }"
         "        .system-info p { color: #6c757d; margin: 5px 0; }"
         "        @media (max-width: 600px) {"
         "            .container { padding: 10px; }"
         "            .header h1 { font-size: 2em; }"
         "            .tabs { flex-direction: column; }"
         "            .tab { border-radius: 0; }"
         "            .led-grid { grid-template-columns: 1fr; }"
         "        }"
         "    </style>"
         "</head>"
         "<body>"
         "    <div class=\"container\">"
         "        <div class=\"header\">"
         "            <h1>ESP32 Gemini Assistant</h1>"
         "            <p>Advanced AI-Powered Smart Assistant</p>"
         "        </div>"
         "        "
         "        <div class=\"tabs\">"
         "            <div class=\"tab active\" onclick=\"showTab('dashboard')\">Dashboard</div>"
         "            <div class=\"tab\" onclick=\"showTab('heartbeat')\">Heartbeat</div>"
         "            <div class=\"tab\" onclick=\"showTab('leds')\">LEDs</div>"
         "            <div class=\"tab\" onclick=\"showTab('tasks')\">Tasks</div>"
         "            <div class=\"tab\" onclick=\"showTab('gemini')\">Gemini</div>"
         "            <div class=\"tab\" onclick=\"showTab('system')\">System</div>"
         "        </div>"
         "        "
         "        <div class=\"content\">"
         "            <!-- Dashboard Tab -->"
         "            <div id=\"dashboard\" class=\"tab-content active\">"
         "                <div class=\"section\">"
         "                    <h3>System Status</h3>"
         "                    <div class=\"system-info\">"
         "                        <h4>Device Information</h4>"
         "                        <p><span class=\"status-indicator status-online\"></span>Status: Online</p>"
         "                        <p>IP Address: " + WiFi.localIP().toString() + "</p>"
         "                        <p>WiFi SSID: " + WiFi.SSID() + "</p>"
         "                        <p>Uptime: <span id=\"uptime\">--</span></p>"
         "                    </div>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>User Settings</h3>"
         "                    <p>Current Username: <strong>USERNAME_PLACEHOLDER</strong></p>"
         "                    <p class=\"info\">Username can be updated via the System tab.</p>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>Current Heartbeat</h3>"
         "                    <div class=\"heartbeat-display\">"
         "                        <h2>Heart Rate</h2>"
         "                        <div class=\"bpm\" id=\"dashboardBPM\">-- BPM</div>"
         "                    </div>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>Recent Tasks</h3>"
         "                    <div id=\"dashboardTasks\">Loading...</div>"
         "                </div>"
         "            </div>"
         "            "
         "            <!-- Heartbeat Tab -->"
         "            <div id=\"heartbeat\" class=\"tab-content\">"
         "                <div class=\"section\">"
         "                    <h3>Heartbeat Monitor</h3>"
         "                    <div class=\"heartbeat-display\">"
         "                        <h2>Current Heart Rate</h2>"
         "                        <div class=\"bpm\" id=\"heartbeatBPM\">-- BPM</div>"
         "                    </div>"
         "                    <button onclick=\"updateHeartbeat()\">Update Reading</button>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>Heart Rate History</h3>"
         "                    <p>Heart rate monitoring with LED synchronization</p>"
         "                    <p>Sensor automatically activates when in heartbeat menu</p>"
         "                </div>"
         "            </div>"
         "            "
         "            <!-- LEDs Tab -->"
         "            <div id=\"leds\" class=\"tab-content\">"
         "                <div class=\"section\">"
         "                    <h3>LED Control</h3>"
         "                    <div class=\"led-grid\">"
         "                        <button class=\"led-btn\" onclick=\"controlLED(0)\">LED 1</button>"
         "                        <button class=\"led-btn\" onclick=\"controlLED(1)\">LED 2</button>"
         "                        <button class=\"led-btn\" onclick=\"controlLED(2)\">LED 3</button>"
         "                        <button class=\"led-btn\" onclick=\"controlLED(3)\">LED 4</button>"
         "                    </div>"
         "                    <button onclick=\"allLEDsOff()\" class=\"btn-secondary\">All LEDs Off</button>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>LED Patterns</h3>"
         "                    <p>Use the OLED menu to access advanced LED patterns:</p>"
         "                    <ul style=\"margin-left: 20px; color: #666;\">"
         "                        <li>Wave Pattern</li>"
         "                        <li>Chase Pattern</li>"
         "                        <li>Breathing Effect</li>"
         "                        <li>Rainbow Cycle</li>"
         "                    </ul>"
         "                </div>"
         "            </div>"
         "            "
         "            <!-- Tasks Tab -->"
         "            <div id=\"tasks\" class=\"tab-content\">"
         "                <div class=\"section\">"
         "                    <h3>Task Management</h3>"
         "                    <div id=\"tasksList\">Loading tasks...</div>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>Add New Task</h3>"
         "                    <div class=\"form-group\">"
         "                        <label>Task Title</label>"
         "                        <input type=\"text\" id=\"taskTitle\" placeholder=\"Enter task title\">"
         "                    </div>"
         "                    <div class=\"form-group\">"
         "                        <label>Description</label>"
         "                        <textarea id=\"taskDesc\" placeholder=\"Enter task description\"></textarea>"
         "                    </div>"
         "                    <div class=\"form-group\">"
         "                        <label>Scheduled Time</label>"
         "                        <input type=\"time\" id=\"taskTime\">"
         "                    </div>"
         "                    <button onclick=\"createTask()\">Add Task</button>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>Alarm Settings</h3>"
         "                    <div id=\"alarmList\">Loading alarms...</div>"
         "                    <div class=\"form-group\">"
         "                        <label>Alarm Time</label>"
         "                        <input type=\"time\" id=\"alarmTime\" placeholder=\"Set alarm time\">"
         "                    </div>"
         "                    <div class=\"form-group\">"
         "                        <label>Alarm Message</label>"
         "                        <input type=\"text\" id=\"alarmMessage\" placeholder=\"Enter alarm message\">"
         "                    </div>"
         "                    <button onclick=\"setAlarm()\">Set Alarm</button>"
         "                    <button onclick=\"testAlarm()\" class=\"btn-secondary\">Test Alarm</button>"
         "                </div>"
         "            </div>"
         "            "
         "            <!-- Gemini Tab -->"
         "            <div id=\"gemini\" class=\"tab-content\">"
         "                <div class=\"section\">"
         "                    <h3>Gemini AI Assistant</h3>"
         "                    <p>Chat with Google's Gemini AI powered by advanced language models</p>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>Chat Interface</h3>"
         "                    <div class=\"form-group\">"
         "                        <label>Your Message</label>"
         "                        <textarea id=\"geminiInput\" placeholder=\"Ask Gemini anything...\" rows=\"4\" onkeydown=\"if(event.key==='Enter' && !event.shiftKey){event.preventDefault(); sendToGemini();}\"></textarea>"
         "                    </div>"
         "                    <button onclick=\"sendToGemini()\">Send to Gemini</button>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>AI Response</h3>"
         "                    <div id=\"geminiResponse\" style=\"background: #f8f9fa; padding: 15px; border-radius: 8px; min-height: 100px; border-left: 4px solid #667eea;\">"
         "                        <p style=\"color: #666; font-style: italic;\">AI responses will appear here...</p>"
         "                    </div>"
         "                </div>"
         "            </div>"
         "            "
         "            <!-- System Tab -->"
         "            <div id=\"system\" class=\"tab-content\">"
         "                <div class=\"section\">"
         "                    <h3>WiFi Configuration</h3>"
         "                    <div class=\"system-info\">"
         "                        <h4>Current Network</h4>"
         "                        <p>SSID: " + WiFi.SSID() + "</p>"
         "                        <p>IP Address: " + WiFi.localIP().toString() + "</p>"
         "                    </div>"
         "                    <form onsubmit=\"updateWiFi(); return false;\">"
         "                        <div class=\"form-group\">"
         "                            <label>New WiFi SSID</label>"
         "                            <input type=\"text\" id=\"newSSID\" placeholder=\"Enter new WiFi name\" autocomplete=\"off\">"
         "                        </div>"
         "                        <div class=\"form-group\">"
         "                            <label>New WiFi Password</label>"
         "                            <input type=\"password\" id=\"newPassword\" placeholder=\"Enter new WiFi password\" autocomplete=\"new-password\">"
         "                        </div>"
         "                        <button type=\"submit\">Update WiFi</button>"
         "                    </form>"
         "                    <button onclick=\"scanWiFi()\" class=\"btn-secondary\">Scan Networks</button>"
         "                    <div id=\"wifiScanResults\"></div>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>User Settings</h3>"
         "                    <div class=\"form-group\">"
         "                        <label>Username</label>"
         "                        <input type=\"text\" id=\"userName\" value=\"USERNAME_PLACEHOLDER\" placeholder=\"Enter your name\" />"
         "                    </div>"
         "                    <button onclick=\"updateUserName()\">Update Username</button>"
         "                    <p class=\"info\">This will personalize your greetings and welcome messages.</p>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>Time Settings</h3>"
         "                    <div class=\"system-info\">"
         "                        <h4>Current Time</h4>"
         "                        <p id=\"currentTime\">Loading...</p>"
         "                    </div>"
         "                    <form onsubmit=\"setTime(); return false;\">"
         "                        <div class=\"form-group\">"
         "                            <label>Set Date & Time</label>"
         "                            <input type=\"datetime-local\" id=\"timeInput\" required>"
         "                        </div>"
         "                        <button type=\"submit\">Update Time</button>"
         "                    </form>"
         "                    <button onclick=\"syncTime()\" class=\"btn-secondary\">Sync with Internet</button>"
         "                    <p class=\"info\">Set the correct time for accurate task scheduling and greetings.</p>"
         "                </div>"
         "                "
         "                <div class=\"section\">"
         "                    <h3>System Management</h3>"
         "                    <button onclick=\"openOTA()\" class=\"btn-success\">OTA Update</button>"
         "                    <button onclick=\"restartDevice()\" class=\"btn-secondary\">Restart Device</button>"
         "                    <button onclick=\"factoryReset()\" class=\"btn-danger\">Factory Reset</button>"
         "                </div>"
         "            </div>"
        "        </div>"
         "    </div>"
         "    "
         "    <script>"
         "        function showTab(tabName) {"
         "            var contents = document.querySelectorAll('.tab-content');"
         "            for (var i = 0; i < contents.length; i++) {"
         "                contents[i].classList.remove('active');"
         "            }"
         "            var tabs = document.querySelectorAll('.tab');"
         "            for (var i = 0; i < tabs.length; i++) {"
         "                tabs[i].classList.remove('active');"
         "            }"
         "            var targetContent = document.getElementById(tabName);"
         "            if (targetContent) {"
         "                targetContent.classList.add('active');"
         "            }"
         "            var clickedTab = event.target;"
         "            if (clickedTab) {"
         "                clickedTab.classList.add('active');"
         "            }"
         "        }"
         "        "
         "        function updateHeartbeat() {"
         "            fetch('/api/heartbeat')"
         "                .then(function(response) { return response.json(); })"
         "                .then(function(data) {"
         "                    document.getElementById('heartbeatBPM').textContent = data.bpm + ' BPM';"
         "                    document.getElementById('dashboardBPM').textContent = data.bpm + ' BPM';"
         "                });"
         "        }"
         "        "
         "        function controlLED(ledIndex) {"
         "            fetch('/api/led', {"
         "                method: 'POST',"
         "                headers: {'Content-Type': 'application/json'},"
         "                body: JSON.stringify({led: ledIndex})"
         "            });"
         "        }"
         "        "
         "        function allLEDsOff() {"
         "            fetch('/api/led', {"
         "                method: 'POST',"
         "                headers: {'Content-Type': 'application/json'},"
         "                body: JSON.stringify({allOff: true})"
         "            });"
         "        }"
         "        "
         "        function loadTasks() {"
         "            fetch('/api/tasks')"
         "                .then(function(response) { return response.json(); })"
         "                .then(function(data) {"
         "                    var tasksDiv = document.getElementById('tasksList');"
         "                    tasksDiv.innerHTML = '';"
         "                    if(data.tasks.length === 0) {"
         "                        tasksDiv.innerHTML = '<p style=\"color: #666; font-style: italic;\">No tasks scheduled</p>';"
         "                    } else {"
         "                        for(var i = 0; i < data.tasks.length; i++) {"
         "                            var task = data.tasks[i];"
         "                            var taskDiv = document.createElement('div');"
         "                            taskDiv.className = 'task-item';"
         "                            var minuteStr = task.minute.toString();"
         "                            if(minuteStr.length === 1) minuteStr = '0' + minuteStr;"
         "                            taskDiv.innerHTML = '<h4>' + task.title + '</h4><p>' + task.description + '</p><p class=\"time\">Time: ' + task.hour + ':' + minuteStr + '</p><button onclick=\"deleteTask(' + i + ')\" class=\"btn-danger\">Delete</button>';"
         "                            tasksDiv.appendChild(taskDiv);"
         "                        }"
         "                    }"
         "                });"
         "        }"
         "        "
         "        function createTask() {"
         "            var title = document.getElementById('taskTitle').value;"
         "            var desc = document.getElementById('taskDesc').value;"
         "            var time = document.getElementById('taskTime').value;"
         "            if(!title || !time) {"
         "                alert('Please fill in title and time');"
         "                return;"
         "            }"
         "            var timeParts = time.split(':');"
         "            var hour = timeParts[0];"
         "            var minute = timeParts[1];"
         "            fetch('/api/tasks', {"
         "                method: 'POST',"
         "                headers: {'Content-Type': 'application/json'},"
         "                body: JSON.stringify({title: title, description: desc, hour: parseInt(hour), minute: parseInt(minute)})"
         "            }).then(function() {"
         "                loadTasks();"
         "                document.getElementById('taskTitle').value = '';"
         "                document.getElementById('taskDesc').value = '';"
         "                document.getElementById('taskTime').value = '';"
         "            });"
         "        }"
         "        "
         "        function deleteTask(index) {"
         "            if(confirm('Are you sure you want to delete this task?')) {"
         "                fetch('/api/tasks', {"
         "                    method: 'DELETE',"
        "                    headers: {'Content-Type': 'application/json'},"
         "                    body: JSON.stringify({index: index})"
         "                }).then(function() { loadTasks(); });"
        "            }"
        "        }"
         "        "
         "        function sendToGemini() {"
         "            var input = document.getElementById('geminiInput').value;"
         "            if(!input.trim()) {"
         "                alert('Please enter a message');"
         "                document.getElementById('geminiInput').focus();"
         "                return;"
         "            }"
         "            var responseDiv = document.getElementById('geminiResponse');"
         "            var button = document.querySelector('button[onclick=\"sendToGemini()\"]');"
         "            responseDiv.innerHTML = '<p style=\"color: #667eea;\">Thinking...</p>';"
         "            button.disabled = true;"
         "            button.textContent = 'Sending...';"
         "            fetch('/api/gemini', {"
         "                method: 'POST',"
         "                headers: {'Content-Type': 'application/json'},"
         "                body: JSON.stringify({message: input})"
         "            }).then(function(response) { return response.json(); })"
         "            .then(function(data) {"
         "                if(data.response) {"
         "                    responseDiv.innerHTML = '<h4>Gemini Response:</h4><p style=\"white-space: pre-wrap;\">' + data.response + '</p>';"
         "                } else {"
         "                    responseDiv.innerHTML = '<p style=\"color: #dc3545;\">Error: No response received</p>';"
         "                }"
         "                document.getElementById('geminiInput').value = '';"
         "            }).catch(function(error) {"
         "                responseDiv.innerHTML = '<p style=\"color: #dc3545;\">Error: ' + error.message + '</p>';"
         "            }).finally(function() {"
         "                button.disabled = false;"
         "                button.textContent = 'Send to Gemini';"
         "                document.getElementById('geminiInput').focus();"
         "            });"
        "        }"
        "        "
        "        function updateWiFi() {"
         "            var ssid = document.getElementById('newSSID').value;"
         "            var password = document.getElementById('newPassword').value;"
        "            if(!ssid || !password) {"
        "                alert('Please enter both SSID and password');"
        "                return;"
        "            }"
         "            var button = event.target;"
         "            var originalText = button.textContent;"
         "            button.textContent = 'Connecting...';"
         "            button.disabled = true;"
         "            fetch('/api/wifi', {"
         "                method: 'POST',"
         "                headers: {'Content-Type': 'application/json'},"
         "                body: JSON.stringify({ssid: ssid, password: password})"
         "            }).then(function(response) { return response.json(); })"
         "            .then(function(data) {"
         "                if(data.status === 'wifi_connected') {"
         "                    alert('WiFi connected successfully! New IP: ' + data.ip + '\\nDevice will restart.');"
        "                    document.getElementById('newSSID').value = '';"
        "                    document.getElementById('newPassword').value = '';"
         "                } else if(data.error === 'connection_failed') {"
         "                    alert('Connection failed: ' + data.message);"
         "                } else {"
         "                    alert('Error: ' + data.error);"
         "                }"
         "            }).catch(function(error) {"
         "                alert('Network error: ' + error.message);"
         "            }).finally(function() {"
         "                button.textContent = originalText;"
         "                button.disabled = false;"
         "            });"
        "        }"
        "        "
        "        function scanWiFi() {"
        "            fetch('/api/wifi/scan')"
         "                .then(function(response) { return response.json(); })"
         "                .then(function(data) {"
         "                    var resultsDiv = document.getElementById('wifiScanResults');"
        "                    resultsDiv.innerHTML = '<h4>Available Networks:</h4>';"
        "                    if(data.networks && data.networks.length > 0) {"
         "                        for(var i = 0; i < data.networks.length; i++) {"
         "                            var network = data.networks[i];"
         "                            var networkDiv = document.createElement('div');"
         "                            networkDiv.innerHTML = '<strong>' + network.ssid + '</strong> (Signal: ' + network.rssi + 'dBm) <button onclick=\"selectNetwork(\\'' + network.ssid + '\\')\" class=\"btn-secondary\">Select</button>';"
        "                            resultsDiv.appendChild(networkDiv);"
         "                        }"
        "                    } else {"
        "                        resultsDiv.innerHTML += '<p>No networks found</p>';"
        "                    }"
        "                });"
        "        }"
        "        "
        "        function selectNetwork(ssid) {"
        "            document.getElementById('newSSID').value = ssid;"
        "            document.getElementById('newPassword').focus();"
         "        }"
         "        "
         "        function openOTA() {"
         "            var ip = '" + WiFi.localIP().toString() + "';"
         "            window.open('http://' + ip + '/update', '_blank');"
         "        }"
         "        "
         "        function restartDevice() {"
         "            if(confirm('Are you sure you want to restart the device?')) {"
         "                fetch('/api/restart', {method: 'POST'});"
         "            }"
         "        }"
         "        "
         "        function updateUserName() {"
         "            var userName = document.getElementById('userName').value;"
         "            if(!userName.trim()) {"
         "                alert('Please enter a username');"
         "                return;"
         "            }"
         "            fetch('/api/username', {"
         "                method: 'POST',"
         "                headers: {'Content-Type': 'application/json'},"
         "                body: JSON.stringify({username: userName})"
         "            }).then(function(response) { return response.json(); })"
         "            .then(function(data) {"
         "                if(data.success) {"
         "                    alert('Username updated successfully!');"
         "                } else {"
         "                    alert('Failed to update username: ' + data.error);"
         "                }"
         "            });"
        "        }"
        "        "
        "        function factoryReset() {"
        "            if(confirm('This will erase all settings and restart. Are you sure?')) {"
        "                if(confirm('This action cannot be undone. Continue?')) {"
        "                    fetch('/api/factory-reset', {method: 'POST'});"
        "                }"
        "            }"
         "        }"
         "        "
         "        function setTime() {"
         "            var timeInput = document.getElementById('timeInput').value;"
         "            if(!timeInput) {"
         "                alert('Please select a date and time');"
         "                return;"
         "            }"
         "            var timestamp = new Date(timeInput).getTime() / 1000;"
         "            fetch('/api/time', {"
         "                method: 'POST',"
         "                headers: {'Content-Type': 'application/json'},"
         "                body: JSON.stringify({timestamp: timestamp})"
         "            }).then(function(response) { return response.json(); })"
         "            .then(function(data) {"
         "                if(data.success) {"
         "                    alert('Time updated successfully!');"
         "                    updateCurrentTime();"
         "                } else {"
         "                    alert('Failed to update time: ' + data.error);"
         "                }"
         "            });"
         "        }"
         "        "
         "        function syncTime() {"
         "            fetch('/api/time/ntp', {method: 'POST'})"
         "            .then(function(response) { return response.json(); })"
         "            .then(function(data) {"
         "                if(data.success) {"
         "                    alert('Time synchronized successfully!');"
         "                    updateCurrentTime();"
         "                } else {"
         "                    alert('Failed to sync time: ' + data.error);"
         "                }"
         "            });"
         "        }"
         "        "
         "        function updateCurrentTime() {"
         "            var now = new Date();"
         "            var timeString = now.toLocaleString();"
         "            document.getElementById('currentTime').textContent = timeString;"
        "        }"
         "        "
        "        loadTasks();"
        "        updateHeartbeat();"
         "        updateCurrentTime();"
         "        setInterval(updateHeartbeat, 5000);"
         "        setInterval(updateCurrentTime, 1000);"
         "    </script>"
         "</body>"
         "</html>";
  
  // Replace username placeholder with actual username
  html.replace("USERNAME_PLACEHOLDER", userName);
  return html;
}

// API Handlers
void handleGetTasks() {
  DynamicJsonDocument doc(JSON_BUFFER_SIZE);
  JsonArray taskArray = doc.createNestedArray("tasks");
  
  for(int i = 0; i < taskCount; i++) {
    JsonObject task = taskArray.createNestedObject();
    task["title"] = tasks[i].title;
    task["description"] = tasks[i].description;
    task["hour"] = tasks[i].hour;
    task["minute"] = tasks[i].minute;
    task["completed"] = tasks[i].completed;
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleCreateTask() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    deserializeJson(doc, server.arg("plain"));
    
    if(taskCount < MAX_TASKS) {
      tasks[taskCount].title = doc["title"].as<String>();
      tasks[taskCount].description = doc["description"].as<String>();
      tasks[taskCount].hour = doc["hour"];
      tasks[taskCount].minute = doc["minute"];
      tasks[taskCount].completed = false;
      tasks[taskCount].due = false;
      taskCount++;
      
      saveTasks();
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"Task limit reached\"}");
    }
  }
}

void handleUpdateTask() {
  server.send(200, "application/json", "{\"status\":\"success\"}");
}

void handleDeleteTask() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    deserializeJson(doc, server.arg("plain"));
    int index = doc["index"];
    
    if(index >= 0 && index < taskCount) {
      for(int i = index; i < taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];
      }
      taskCount--;
      saveTasks();
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"Invalid index\"}");
    }
  }
}

void handleGeminiRequest() {
  Serial.println("=== Gemini Request Received ===");
  Serial.println("Raw request: " + server.arg("plain"));
  
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    deserializeJson(doc, server.arg("plain"));
    String userText = doc["message"].as<String>();
    
    Serial.println("Extracted user text: " + userText);
    
    // Send request to Gemini API
    String response = callGeminiAPI(userText);
    
    Serial.println("Gemini response: " + response);
    
    DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
    responseDoc["response"] = response;
    
    String responseString;
    serializeJson(responseDoc, responseString);
    server.send(200, "application/json", responseString);
    
    // Trigger happy animation
    animationState = ANIM_HAPPY;
    currentFrame = 0;
    lastAnimationTime = millis();
    
    // Play success sound with LED animation
    playGeminiResponseSound();
    
    // Flash all LEDs for Gemini response
    for(int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(200);
    for(int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], LOW);
    }
  }
}

void handleGetHeartbeat() {
  DynamicJsonDocument doc(256);
  doc["bpm"] = currentBPM;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleLEDControl() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    int ledIndex = doc["led"];
    String state = doc["state"].as<String>();
    
    if(ledIndex == -1 && state == "off") {
      // Turn off all LEDs
      for(int i = 0; i < 4; i++) {
        digitalWrite(ledPins[i], LOW);
        ledStates[i] = LOW;
      }
    } else if(ledIndex >= 0 && ledIndex < 4) {
      if(state == "toggle") {
        ledStates[ledIndex] = !ledStates[ledIndex];
        digitalWrite(ledPins[ledIndex], ledStates[ledIndex]);
      } else if(state == "on") {
        ledStates[ledIndex] = HIGH;
        digitalWrite(ledPins[ledIndex], HIGH);
      } else if(state == "off") {
        ledStates[ledIndex] = LOW;
        digitalWrite(ledPins[ledIndex], LOW);
      }
    }
    
    server.send(200, "application/json", "{\"status\":\"success\"}");
  }
}

void handleAlarm() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    if(doc["test"]) {
      // Test alarm sound
      playNotificationSound();
      server.send(200, "application/json", "{\"status\":\"test_complete\"}");
    } else if(doc["clearAll"]) {
      // Clear all alarms
      int newTaskCount = 0;
      for(int i = 0; i < taskCount; i++) {
        if(!tasks[i].title.startsWith("ALARM:")) {
          tasks[newTaskCount] = tasks[i];
          newTaskCount++;
        }
      }
      taskCount = newTaskCount;
      saveTasks();
      server.send(200, "application/json", "{\"status\":\"alarms_cleared\"}");
    } else {
      // Set alarm (store in SPIFFS)
      int hour = doc["hour"];
      int minute = doc["minute"];
      String message = doc["message"].as<String>();
      
      // Create alarm task
      if(taskCount < MAX_TASKS) {
        tasks[taskCount].title = "ALARM: " + message;
        tasks[taskCount].description = message;
        tasks[taskCount].hour = hour;
        tasks[taskCount].minute = minute;
        tasks[taskCount].completed = false;
        tasks[taskCount].due = false;
        taskCount++;
        
        saveTasks();
        server.send(200, "application/json", "{\"status\":\"alarm_set\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"Task limit reached\"}");
      }
    }
  }
}

void handleRestart() {
  server.send(200, "application/json", "{\"status\":\"restarting\"}");
  delay(1000);
  ESP.restart();
}

void handleWiFiUpdate() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    String newSSID = doc["ssid"].as<String>();
    String newPassword = doc["password"].as<String>();
    
    // Test WiFi connection before saving
    Serial.println("Testing WiFi connection to: " + newSSID);
    
    // Disconnect from current WiFi
    WiFi.disconnect();
    delay(1000);
    
    // Try to connect to new WiFi
    WiFi.begin(newSSID.c_str(), newPassword.c_str());
    
    int attempts = 0;
    const int maxAttempts = 5;
    
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
      delay(1000);
      attempts++;
      Serial.println("Connection attempt " + String(attempts) + "/" + String(maxAttempts));
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connection successful!");
    
    // Save WiFi credentials to SPIFFS
    DynamicJsonDocument wifiDoc(512);
    wifiDoc["ssid"] = newSSID;
    wifiDoc["password"] = newPassword;
    
    File file = SPIFFS.open("/wifi_config.json", "w");
    if(file) {
      serializeJson(wifiDoc, file);
      file.close();
      
        server.send(200, "application/json", "{\"status\":\"wifi_connected\",\"ip\":\"" + WiFi.localIP().toString() + "\"}");
      
      // Restart after a delay to allow response to be sent
      delay(2000);
      ESP.restart();
    } else {
      server.send(500, "application/json", "{\"error\":\"failed_to_save\"}");
    }
    } else {
      Serial.println("WiFi connection failed!");
      server.send(400, "application/json", "{\"error\":\"connection_failed\",\"message\":\"Unable to connect to the specified WiFi network. Please check your credentials and try again.\"}");
      
      // Reconnect to current WiFi or AP mode
      WiFi.disconnect();
      delay(1000);
      
      // Try to reconnect to previous WiFi
      // Load previous WiFi credentials from SPIFFS
      File wifiFile = SPIFFS.open("/wifi_config.json", "r");
      if (wifiFile) {
        DynamicJsonDocument prevWifiDoc(512);
        deserializeJson(prevWifiDoc, wifiFile);
        wifiFile.close();
        
        String prevSSID = prevWifiDoc["ssid"].as<String>();
        String prevPassword = prevWifiDoc["password"].as<String>();
        
        if (prevSSID.length() > 0) {
          WiFi.begin(prevSSID.c_str(), prevPassword.c_str());
        } else {
          // Start AP mode if no previous WiFi
          WiFi.mode(WIFI_AP);
          WiFi.softAP("Mini Gem v1", "gemini123");
        }
      } else {
        // Start AP mode if no previous WiFi
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Mini Gem v1", "gemini123");
      }
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"no_data\"}");
  }
}

void handleWiFiScan() {
  DynamicJsonDocument doc(2048);
  JsonArray networks = doc.createNestedArray("networks");
  
  int n = WiFi.scanNetworks();
  
  for(int i = 0; i < n; i++) {
    JsonObject network = networks.createNestedObject();
    network["ssid"] = WiFi.SSID(i);
    network["rssi"] = WiFi.RSSI(i);
    network["encryption"] = WiFi.encryptionType(i);
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Username Update Handler
void handleUserNameUpdate() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    deserializeJson(doc, server.arg("plain"));
    
    if(doc.containsKey("username")) {
      String newUserName = doc["username"];
      newUserName.trim();
      
      if(newUserName.length() > 0 && newUserName.length() <= 20) {
        userName = newUserName;
        saveUserName();
        server.send(200, "application/json", "{\"success\":true,\"username\":\"" + userName + "\"}");
      } else {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Username must be 1-20 characters\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"username_required\"}");
    }
  } else {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"no_data\"}");
  }
}

void handleFactoryReset() {
  server.send(200, "application/json", "{\"status\":\"factory_reset\"}");
  
  // Clear all global variables
  taskCount = 0;
  heartbeatSensorActive = false;
  lampMode = false;
  currentState = STATE_IDLE;
  currentMenuOption = 0;
  currentBPM = 0;
  hasDueTask = false;
  userName = "User";
  welcomeMessage = "";
  
  // Clear task array
  for(int i = 0; i < MAX_TASKS; i++) {
    tasks[i].title = "";
    tasks[i].description = "";
    tasks[i].hour = 0;
    tasks[i].minute = 0;
    tasks[i].completed = false;
  }
  
  // Turn off all LEDs
  for(int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  
  // Turn off buzzer
  noTone(BUZZER_PIN);
  
  // Turn off heartbeat sensor power
  digitalWrite(HEARTBEAT_POWER_PIN, LOW);
  
  // Clear all files in SPIFFS
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file) {
    SPIFFS.remove(file.name());
    file = root.openNextFile();
  }
  
  // Format SPIFFS to ensure complete reset
  SPIFFS.format();
  
  delay(2000);
  ESP.restart();
}

// Time Synchronization Handlers
void handleTimeSync() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    deserializeJson(doc, server.arg("plain"));
    
    if(doc.containsKey("timestamp")) {
      unsigned long timestamp = doc["timestamp"];
      
      // Set system time using configTime
      struct timeval tv;
      tv.tv_sec = timestamp;
      tv.tv_usec = 0;
      settimeofday(&tv, NULL);
      
      server.send(200, "application/json", "{\"success\":true,\"timestamp\":" + String(timestamp) + "}");
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"timestamp_required\"}");
    }
  } else {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"no_data\"}");
  }
}

void handleNTPTimeSync() {
  // Sync with NTP server - set timezone offset (adjust for your timezone)
  // For example: GMT+5:30 = 5*3600 + 30*60 = 19800 seconds
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov", "time.google.com");
  
  Serial.println("Starting NTP sync...");
  
  int attempts = 0;
  while(time(nullptr) < 1000000000 && attempts < 15) {
    delay(1000);
    attempts++;
    Serial.println("NTP sync attempt " + String(attempts) + ", time: " + String(time(nullptr)));
  }
  
  if(time(nullptr) > 1000000000) {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    Serial.println("NTP sync successful! Time: " + String(timeinfo->tm_hour) + ":" + String(timeinfo->tm_min));
    server.send(200, "application/json", "{\"success\":true,\"timestamp\":" + String(time(nullptr)) + "}");
  } else {
    Serial.println("NTP sync failed after 15 attempts");
    server.send(500, "application/json", "{\"success\":false,\"error\":\"ntp_sync_failed\"}");
  }
}

// Gemini API Integration
String callGeminiAPI(String userText) {
  WiFiClientSecure client;
  client.setInsecure(); // For development only - use proper certificates in production
  
  if(!client.connect("generativelanguage.googleapis.com", 443)) {
    return "Gemini not connected";
  }
  
  // Escape special characters in user text
  userText.replace("\"", "\\\"");
  userText.replace("\n", "\\n");
  userText.replace("\r", "\\r");
  userText.replace("\t", "\\t");
  
  String requestBody = "{\"contents\":[{\"parts\":[{\"text\":\"" + userText + "\"}]}]}";
  
  // Debug: Print request details
  Serial.println("=== Gemini API Request ===");
  Serial.println("Request Body: " + requestBody);
  Serial.println("API Key: " + String(GEMINI_API_KEY));
  Serial.println("API Key Length: " + String(strlen(GEMINI_API_KEY)));
  Serial.println("User Text: " + userText);
  Serial.println("=========================");
  
  // Try gemini-1.5-flash first (this is a valid endpoint)
  String httpRequest = "POST /v1beta/models/gemini-1.5-flash:generateContent?key=" + String(GEMINI_API_KEY) + " HTTP/1.1\r\n";
  httpRequest += "Host: generativelanguage.googleapis.com\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: " + String(requestBody.length()) + "\r\n";
  httpRequest += "Connection: close\r\n\r\n";
  httpRequest += requestBody;
  
  client.print(httpRequest);
  
  String response = "";
  unsigned long timeout = millis() + 15000; // 15 second timeout for larger responses
  bool readingHeaders = true;
  bool readingChunked = false;
  int chunkSize = 0;
  
  while(client.connected() && millis() < timeout) {
    if(client.available()) {
      if(readingHeaders) {
        String line = client.readStringUntil('\n');
        response += line + "\n";
        
        // Check if we're done with headers
        if(line.length() <= 1) { // Empty line indicates end of headers
          readingHeaders = false;
          // Check if response is chunked
          if(response.indexOf("Transfer-Encoding: chunked") >= 0) {
            readingChunked = true;
          }
        }
      } else if(readingChunked) {
        // Read chunk size
        String chunkSizeLine = client.readStringUntil('\n');
        chunkSize = strtol(chunkSizeLine.c_str(), NULL, 16);
        
        if(chunkSize == 0) {
          break; // End of chunks
        }
        
        // Read chunk data
        String chunkData = "";
        int bytesRead = 0;
        while(bytesRead < chunkSize && client.available()) {
          chunkData += (char)client.read();
          bytesRead++;
        }
        response += chunkData;
        
        // Skip the trailing CRLF after chunk
        client.readStringUntil('\n');
      } else {
        // Regular response reading
        response += client.readString();
      }
    }
    delay(5);
  }
  
  // If client is still connected, read any remaining data
  if(client.connected()) {
    while(client.available()) {
      response += client.readString();
      delay(5);
    }
  }
  
  // Debug: Print raw response
  Serial.println("=== Gemini API Response ===");
  Serial.println("Response Length: " + String(response.length()));
  Serial.println("Response Complete: " + String(response.endsWith("}") ? "Yes" : "No"));
  Serial.println("Raw Response: " + response);
  Serial.println("==========================");
  
  // Check if response is HTML (404 error page)
  if(response.indexOf("<!DOCTYPE html>") >= 0 || response.indexOf("<html") >= 0) {
    Serial.println("Received HTML response instead of JSON - likely 404 error");
    return "Error: API endpoint not found (404). Please check API key or try fallback.";
  }
  
  // Parse Gemini API response properly
  DynamicJsonDocument doc(8192); // Increased buffer size for larger responses
  DeserializationError error = deserializeJson(doc, response);
  
  if(error) {
    Serial.println("JSON parsing failed: " + String(error.c_str()));
    Serial.println("Response length: " + String(response.length()));
    Serial.println("Response: " + response.substring(0, min(500, (int)response.length())));
    return "Error: Invalid response format - " + String(error.c_str());
  }
  
  // Check for errors in response
  if(doc.containsKey("error")) {
    String errorMsg = doc["error"]["message"];
    int errorCode = doc["error"]["code"];
    Serial.println("API Error Code: " + String(errorCode));
    Serial.println("API Error Message: " + errorMsg);
    
    // If it's a 404 error, try the older API endpoint
    if(errorCode == 404) {
      Serial.println("Trying fallback API endpoint...");
      return callGeminiAPIFallback(userText);
    }
    
    return "Error " + String(errorCode) + ": " + errorMsg;
  }
  
  // Extract text from Gemini response
  if(doc.containsKey("candidates") && doc["candidates"].is<JsonArray>()) {
    JsonArray candidates = doc["candidates"];
    if(candidates.size() > 0) {
      JsonObject candidate = candidates[0];
      if(candidate.containsKey("content") && candidate["content"].containsKey("parts")) {
        JsonArray parts = candidate["content"]["parts"];
        if(parts.size() > 0) {
          String text = parts[0]["text"];
          Serial.println("Successfully extracted text: " + text);
          return text;
        }
      }
    }
  }
  
  // Debug: Print response structure
  Serial.println("Response structure:");
  serializeJsonPretty(doc, Serial);
  Serial.println();
  
  return "Error: No response text found - check Serial monitor for response structure";
}

// Fallback Gemini API function using older endpoint
String callGeminiAPIFallback(String userText) {
  WiFiClientSecure client;
  client.setInsecure();
  
  if(!client.connect("generativelanguage.googleapis.com", 443)) {
    return "Gemini fallback not connected";
  }
  
  // Escape special characters in user text
  userText.replace("\"", "\\\"");
  userText.replace("\n", "\\n");
  userText.replace("\r", "\\r");
  userText.replace("\t", "\\t");
  
  String requestBody = "{\"contents\":[{\"parts\":[{\"text\":\"" + userText + "\"}]}]}";
  
  Serial.println("=== Gemini Fallback API Request ===");
  Serial.println("Request Body: " + requestBody);
  Serial.println("================================");
  
  // Try the gemini-pro endpoint (fallback)
  String httpRequest = "POST /v1beta/models/gemini-pro:generateContent?key=" + String(GEMINI_API_KEY) + " HTTP/1.1\r\n";
  httpRequest += "Host: generativelanguage.googleapis.com\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: " + String(requestBody.length()) + "\r\n";
  httpRequest += "Connection: close\r\n\r\n";
  httpRequest += requestBody;
  
  client.print(httpRequest);
  
  String response = "";
  unsigned long timeout = millis() + 10000; // 10 second timeout
  while(client.connected() && millis() < timeout) {
    if(client.available()) {
      response += client.readString();
    }
    delay(10); // Small delay to ensure complete reading
  }
  
  // If client is still connected, read any remaining data
  if(client.connected()) {
    while(client.available()) {
      response += client.readString();
      delay(10);
    }
  }
  
  Serial.println("=== Gemini Fallback API Response ===");
  Serial.println("Response Length: " + String(response.length()));
  Serial.println("Raw Response: " + response);
  Serial.println("=================================");
  
  // Check if response is HTML (404 error page)
  if(response.indexOf("<!DOCTYPE html>") >= 0 || response.indexOf("<html") >= 0) {
    Serial.println("Fallback also received HTML response - API key may be invalid");
    return "Error: Both API endpoints failed (404). Please check your API key.";
  }
  
  // Parse response
  DynamicJsonDocument doc(8192); // Increased buffer size for larger responses
  DeserializationError error = deserializeJson(doc, response);
  
  if(error) {
    Serial.println("Fallback JSON parsing failed: " + String(error.c_str()));
    return "Error: Fallback API failed - " + String(error.c_str());
  }
  
  // Check for errors
  if(doc.containsKey("error")) {
    String errorMsg = doc["error"]["message"];
    int errorCode = doc["error"]["code"];
    Serial.println("Fallback API Error Code: " + String(errorCode));
    Serial.println("Fallback API Error Message: " + errorMsg);
    return "Error " + String(errorCode) + ": " + errorMsg;
  }
  
  // Extract text
  if(doc.containsKey("candidates") && doc["candidates"].is<JsonArray>()) {
    JsonArray candidates = doc["candidates"];
    if(candidates.size() > 0) {
      JsonObject candidate = candidates[0];
      if(candidate.containsKey("content") && candidate["content"].containsKey("parts")) {
        JsonArray parts = candidate["content"]["parts"];
        if(parts.size() > 0) {
          String text = parts[0]["text"];
          Serial.println("Fallback successfully extracted text: " + text);
          return text;
        }
      }
    }
  }
  
  return "No response from Gemini fallback";
}

// Display Update Function
void updateDisplay() {
  if(menuOpen) {
    drawMenuScreen();
  } else {
  switch(currentState) {
    case STATE_IDLE:
      drawIdleScreen();
      break;
    case STATE_MENU:
      drawMenuScreen();
      break;
    case STATE_TASK_DISPLAY:
      drawTaskScreen();
      break;
    case STATE_HEARTBEAT_MONITOR:
      drawHeartbeatScreen();
      break;
    case STATE_NOTIFICATION:
      drawNotificationScreen();
      break;
    case STATE_GEMINI_CHAT:
      drawGeminiScreen();
      break;
    case STATE_LAMP_CONTROL:
      drawLampScreen();
      break;
    case STATE_ALARM_CONTROL:
      // Repurposed for System info on OLED
      drawSystemScreen();
      break;
    case STATE_SETTINGS:
      drawSettingsScreen();
      break;
    }
  }
}

void drawIdleScreen() {
  // Static variables to track changes
  static unsigned long lastTimeUpdate = 0;
  static char lastTimeStr[20] = "--:-- --/--";
  static const char* lastStatusText = "Ready";
  static int lastBPM = 0;
  static bool lastHasDueTask = false;
  static unsigned long lastIdleUpdate = 0;
  static unsigned long lastBlinkTime = 0;
  static unsigned long lastEyeMoveTime = 0;
  static int currentEyeAnimation = 0; // 0=normal, 1=blink, 2=left, 3=right, 4=up, 5=down, 6=greeting
  static bool isBlinking = false;
  
  unsigned long currentTime = millis();
  
  // Only update display if something has changed or every 1 second for blinking
  bool needsUpdate = false;
  
  // Check if time needs update
  if(currentTime - lastTimeUpdate > 1000) {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char newTimeStr[20];
    sprintf(newTimeStr, "%02d:%02d %02d/%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_mday, timeinfo->tm_mon + 1);
    if(strcmp(newTimeStr, lastTimeStr) != 0) {
      strcpy(lastTimeStr, newTimeStr);
      needsUpdate = true;
    }
    lastTimeUpdate = currentTime;
  }
  
  // Check if status needs update
  String currentStatusText;
  if(WiFi.status() != WL_CONNECTED) {
    currentStatusText = ""; // No message when in AP mode
  } else if(hasDueTask) {
    currentStatusText = "Alert!";
  } else if(currentBPM > 0) {
    currentStatusText = "Heart";
  } else if(currentEyeAnimation == 6) {
    currentStatusText = getGreetingMessage(); // Show personalized greeting only during greeting animation
  } else {
    currentStatusText = ""; // No text for other animations
  }
  // Removed taskCount check to always show personalized greeting
  
  // Convert to const char* for comparison
  const char* statusCStr = currentStatusText.c_str();
  
  if(strcmp(statusCStr, lastStatusText) != 0) {
    lastStatusText = statusCStr;
    needsUpdate = true;
  }
  
  // Check if BPM changed
  if(currentBPM != lastBPM) {
    lastBPM = currentBPM;
    needsUpdate = true;
  }
  
  // Check if due task status changed
  if(hasDueTask != lastHasDueTask) {
    lastHasDueTask = hasDueTask;
    needsUpdate = true;
  }
  
  // Automatic eye animations every 2 seconds
  if(currentTime - lastEyeMoveTime > 2000) {
    needsUpdate = true;
    currentEyeAnimation = (currentEyeAnimation + 1) % 7; // Cycle through 0-6 (including greeting)
    lastEyeMoveTime = currentTime;
  }
  
  // Blinking every 1 second when in normal state
  if(currentEyeAnimation == 0 && currentTime - lastBlinkTime > 1000) {
    needsUpdate = true;
    isBlinking = !isBlinking;
    lastBlinkTime = currentTime;
  }
  
  // Only redraw if something changed
  if(needsUpdate) {
  u8g2.clearBuffer();
  
    // Draw eyes with automatic animations (same for both WiFi states)
    switch(currentEyeAnimation) {
      case 0: // Normal with blinking
        if(isBlinking) {
          close();
        } else {
          normal();
        }
        break;
      case 1: // Blink
        close();
        break;
      case 2: // Look left
        lefteye();
        break;
      case 3: // Look right
        righteye();
        break;
      case 4: // Look up
        upeye();
        break;
      case 5: // Look down
        downeye();
        break;
      case 6: // Show greeting message
        upeye(); // Show eyes looking up during greeting
        break;
    }
  
  // Show BPM if available (bottom left)
  if(currentBPM > 0) {
    char bpmStr[15];
    sprintf(bpmStr, "BPM: %d", currentBPM);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 60, bpmStr);
  }
  
  // Show WiFi status indicator (top right)
  if(WiFi.status() == WL_CONNECTED) {
    // Draw WiFi icon (simple representation)
    u8g2.drawCircle(120, 8, 3, U8G2_DRAW_ALL); // Outer circle
    u8g2.drawCircle(120, 8, 2, U8G2_DRAW_ALL); // Middle circle
    u8g2.drawCircle(120, 8, 1, U8G2_DRAW_ALL); // Inner circle
    u8g2.drawPixel(120, 8); // Center dot
  }
  
  // Show due task indicator (top right, below WiFi icon)
  if(hasDueTask) {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(110, 20, "!");
  }
  
    // Show time (top left)
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, lastTimeStr);
  
    // Show personalized greeting in center
  u8g2.setFont(u8g2_font_ncenB08_tr);
    int textWidth = u8g2.getStrWidth(currentStatusText.c_str());
    int centerX = (128 - textWidth) / 2; // Center horizontally
    u8g2.drawStr(centerX, 50, currentStatusText.c_str());
  
  u8g2.sendBuffer();
  }
}

void drawMenuScreen() {
  u8g2.clearBuffer();
  
  // No title - just menu items for cleaner display
  u8g2.setFont(u8g2_font_ncenB08_tr);
  
  for(int i = 0; i < maxMenuItems; i++) {
    int y = 10 + (i * 10); // Start at 10, each item 10 pixels apart
    
    // Highlight current option
    if(i == currentMenuItem) {
      u8g2.drawBox(5, y - 8, 118, 10);
      u8g2.setDrawColor(0);
    }
    
    // Draw menu item with proper spacing
    u8g2.drawStr(10, y, menuItems[i].c_str());
    
    // Reset draw color
    u8g2.setDrawColor(1);
  }
  
  u8g2.sendBuffer();
}

void drawTaskScreen() {
  u8g2.clearBuffer();
  
  // Title
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(50, 15, "Tasks");
  
  // Task list with better formatting
  u8g2.setFont(u8g2_font_ncenB08_tr);
  int yPos = 25;
  int maxTasks = MIN(taskCount, 5); // Show up to 5 tasks
  
  for(int i = 0; i < maxTasks; i++) {
    // Task title (truncated if too long)
    String taskLine = tasks[i].title;
    if(taskLine.length() > 15) {
      taskLine = taskLine.substring(0, 12) + "...";
    }
    
    char taskStr[20];
    sprintf(taskStr, "%s", taskLine.c_str());
    u8g2.drawStr(5, yPos, taskStr);
    
    // Due indicator
    if(tasks[i].due) {
      u8g2.drawStr(90, yPos, "!");
    }
    
    // Time
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d", tasks[i].hour, tasks[i].minute);
    u8g2.drawStr(100, yPos, timeStr);
    
    yPos += 10;
  }
  
  // Instructions
  if(taskCount == 0) {
    u8g2.drawStr(10, 35, "No tasks yet");
    u8g2.drawStr(5, 45, "Use web interface");
    u8g2.drawStr(5, 55, "to add tasks");
  } else {
    u8g2.drawStr(0, 60, "Double tap: notifications");
  }
  
  u8g2.sendBuffer();
}

void drawHeartbeatScreen() {
  u8g2.clearBuffer();
  
  // Title centered
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(20, 15, "Heartbeat Monitor");
  
  // Large BPM display in center
  u8g2.setFont(u8g2_font_ncenB14_tr);
  char bpmStr[15];
  sprintf(bpmStr, "BPM: %d", currentBPM);
  u8g2.drawStr(30, 35, bpmStr);
  
  // Heart animation below BPM
  drawHeartAnimation();
  
  // Status information
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 55, "Status: ");
  if(currentBPM > 0) {
    u8g2.drawStr(50, 55, "Detected");
  } else {
    u8g2.drawStr(50, 55, "No signal");
  }
  
  // Instructions
  u8g2.drawStr(0, 65, "Place finger on sensor");
  
  u8g2.sendBuffer();
}

void drawNotificationScreen() {
  u8g2.clearBuffer();
  
  // Title centered
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(20, 15, "NOTIFICATION");
  
  // Notification content
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5, 30, currentNotification.c_str());
  
  // Task details if available
  for(int i = 0; i < taskCount; i++) {
    if(tasks[i].due) {
      u8g2.drawStr(5, 45, "Task:");
      u8g2.drawStr(5, 55, tasks[i].title.c_str());
      break;
    }
  }
  
  // Instructions
  u8g2.drawStr(5, 65, "Tap to dismiss");
  
  u8g2.sendBuffer();
}

void drawGeminiScreen() {
  u8g2.clearBuffer();
  
  // Title centered
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(10, 15, "Gemini Assistant");
  
  // Instructions
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5, 30, "Use web interface");
  u8g2.drawStr(5, 42, "to chat with AI");
  
  // IP address
  u8g2.drawStr(5, 55, "IP:");
  String ipStr = WiFi.localIP().toString();
  u8g2.drawStr(20, 55, ipStr.c_str());
  
  // Status
  u8g2.drawStr(5, 65, "Status:");
  if(WiFi.status() == WL_CONNECTED) {
    u8g2.drawStr(50, 65, "Connected");
  } else {
    u8g2.drawStr(50, 65, "Disconnected");
  }
  
  u8g2.sendBuffer();
}

void drawLampScreen() {
  u8g2.clearBuffer();
  
  // Title centered
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(30, 15, "Lamp Control");
  
  // Current pattern
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5, 30, "Pattern:");
  String patternName = getPatternName(lampPattern);
  u8g2.drawStr(60, 30, patternName.c_str());
  
  // Instructions
  u8g2.drawStr(5, 45, "T1: Next pattern");
  u8g2.drawStr(5, 55, "T2: Start/Stop");
  
  // Status
  u8g2.drawStr(5, 65, lampMode ? "Running" : "Stopped");
  
  u8g2.sendBuffer();
}

void drawSettingsScreen() {
  u8g2.clearBuffer();
  
  // Title centered
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(30, 15, "Settings");
  
  // Username
  u8g2.setFont(u8g2_font_ncenB08_tr);
  String userStr = "User: " + userName;
  u8g2.drawStr(5, 30, userStr.c_str());
  
  // WiFi Status
  String wifiStr = "WiFi: ";
  if(WiFi.status() == WL_CONNECTED) {
    wifiStr += "Connected";
  } else {
    wifiStr += "Disconnected";
  }
  u8g2.drawStr(5, 40, wifiStr.c_str());
  
  // IP Address
  if(WiFi.status() == WL_CONNECTED) {
    String ipStr = "IP: " + WiFi.localIP().toString();
    u8g2.drawStr(5, 50, ipStr.c_str());
  } else {
    u8g2.drawStr(5, 50, "AP Mode: 192.168.4.1");
  }
  
  // Instructions
  u8g2.drawStr(5, 65, "Use web interface");
  
  u8g2.sendBuffer();
}

String getPatternName(int pattern) {
  switch(pattern) {
    case 0: return "Off";
    case 1: return "All On";
    case 2: return "Wave";
    case 3: return "Chase";
    case 4: return "Breathing";
    case 5: return "Rainbow";
    default: return "Unknown";
  }
}

void drawAnimationFrame() {
  // Use advanced eye animations
  switch(animationState) {
    case ANIM_IDLE:
      normal();
      break;
    case ANIM_BLINK:
      blink();
      break;
    case ANIM_HAPPY:
      happy();
      break;
    case ANIM_HEART:
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.drawStr(50, 35, "<3");
      u8g2.sendBuffer();
      break;
    case ANIM_LOOK_LEFT:
      lefteye();
      break;
    case ANIM_LOOK_RIGHT:
      righteye();
      break;
    case ANIM_SAD:
      sad();
      break;
    case ANIM_ANGRY:
      angry();
      break;
    case ANIM_CUTE:
      cute();
      break;
    case ANIM_WONDER:
      wonder();
      break;
  }
}

// System Screen Function
void drawSystemScreen() {
  u8g2.clearBuffer();
  
  // Title
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(40, 15, "System Info");
  
  // WiFi Status
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5, 30, "WiFi:");
  if(WiFi.status() == WL_CONNECTED) {
    u8g2.drawStr(35, 30, "Connected");
    u8g2.drawStr(5, 42, WiFi.SSID().c_str());
  } else {
    u8g2.drawStr(35, 30, "AP Mode");
    u8g2.drawStr(5, 42, "Mini Gem v1");
  }
  
  // IP Address
  u8g2.drawStr(5, 54, "IP:");
  if(WiFi.status() == WL_CONNECTED) {
    u8g2.drawStr(25, 54, WiFi.localIP().toString().c_str());
    } else {
    u8g2.drawStr(25, 54, "192.168.4.1");
  }
  
  // Memory info
  u8g2.drawStr(5, 66, "Free RAM:");
  u8g2.drawStr(65, 66, String(ESP.getFreeHeap()).c_str());
  
  u8g2.sendBuffer();
}

void updateLampPatterns() {
  if(!lampMode) return;
  
  unsigned long currentTime = millis();
  if(currentTime - lastLampUpdate < 100) return; // Update every 100ms
  lastLampUpdate = currentTime;
  
  switch(lampPattern) {
    case 0: // Off
      for(int i = 0; i < 4; i++) {
        digitalWrite(ledPins[i], LOW);
        ledStates[i] = LOW;
      }
      break;
      
    case 1: // All On
      for(int i = 0; i < 4; i++) {
        digitalWrite(ledPins[i], HIGH);
        ledStates[i] = HIGH;
      }
      break;
      
    case 2: // Wave
      for(int i = 0; i < 4; i++) {
        if(i == lampStep) {
          digitalWrite(ledPins[i], HIGH);
          ledStates[i] = HIGH;
        } else {
          digitalWrite(ledPins[i], LOW);
          ledStates[i] = LOW;
        }
      }
      lampStep = (lampStep + 1) % 4;
      break;
      
    case 3: // Chase
      for(int i = 0; i < 4; i++) {
        if(i == lampStep || i == (lampStep + 1) % 4) {
          digitalWrite(ledPins[i], HIGH);
          ledStates[i] = HIGH;
        } else {
          digitalWrite(ledPins[i], LOW);
          ledStates[i] = LOW;
        }
      }
      lampStep = (lampStep + 1) % 4;
      break;
      
    case 4: // Breathing
      {
        int brightness = (sin(millis() * 0.005) + 1) * 50; // 0-100%
        for(int i = 0; i < 4; i++) {
          if(brightness > 50) {
            digitalWrite(ledPins[i], HIGH);
            ledStates[i] = HIGH;
          } else {
            digitalWrite(ledPins[i], LOW);
            ledStates[i] = LOW;
          }
        }
      }
      break;
      
    case 5: // Rainbow (cycling through LEDs)
      for(int i = 0; i < 4; i++) {
        if(i == lampStep) {
          digitalWrite(ledPins[i], HIGH);
          ledStates[i] = HIGH;
        } else {
          digitalWrite(ledPins[i], LOW);
          ledStates[i] = LOW;
        }
      }
      lampStep = (lampStep + 1) % 4;
      break;
  }
}

void handleLampTouch() {
  static unsigned long lastTouchTime1 = 0;
  static unsigned long lastTouchTime2 = 0;
  static bool touchPressed1 = false;
  static bool touchPressed2 = false;
  unsigned long currentTime = millis();
  
  // Touch Sensor 1 - Next pattern
  bool currentTouch1 = (digitalRead(TOUCH_PIN_1) == HIGH);
  if(currentTouch1 && !touchPressed1) {
    // Add debounce delay - only trigger if touch is stable for 50ms
    if(currentTime - lastTouchTime1 > 50) {
      touchPressed1 = true;
      lastTouchTime1 = currentTime;
      
      // Next pattern
      lampPattern = (lampPattern + 1) % 6;
      lampStep = 0;
      Serial.println("Lamp pattern: " + String(lampPattern));
    }
  }
  if(!currentTouch1 && touchPressed1) {
    touchPressed1 = false;
    lastTouchTime1 = currentTime; // Update last touch time on release
  }
  
  // Touch Sensor 2 - Start/Stop
  bool currentTouch2 = (digitalRead(TOUCH_PIN_2) == HIGH);
  if(currentTouch2 && !touchPressed2) {
    // Add debounce delay - only trigger if touch is stable for 50ms
    if(currentTime - lastTouchTime2 > 50) {
      touchPressed2 = true;
      lastTouchTime2 = currentTime;
      
      // Toggle lamp mode
      lampMode = !lampMode;
      lampStep = 0;
      Serial.println("Lamp mode: " + String(lampMode ? "ON" : "OFF"));
    }
  }
  if(!currentTouch2 && touchPressed2) {
    touchPressed2 = false;
    lastTouchTime2 = currentTime; // Update last touch time on release
  }
}

// Alarm Control Functions
void drawAlarmScreen() {
  u8g2.clearBuffer();
  
  // Title
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(40, 15, "Alarm Control");
  
  // Current alarm status
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5, 30, "Status:");
  
  // Check if any alarms are set
  bool hasAlarms = false;
  for(int i = 0; i < taskCount; i++) {
    if(tasks[i].title.startsWith("ALARM:")) {
      hasAlarms = true;
      break;
    }
  }
  
  if(hasAlarms) {
    u8g2.drawStr(50, 30, "Alarms Set");
  } else {
    u8g2.drawStr(50, 30, "No Alarms");
  }
  
  // Show next alarm
  u8g2.drawStr(5, 42, "Next Alarm:");
  bool foundNext = false;
  for(int i = 0; i < taskCount; i++) {
    if(tasks[i].title.startsWith("ALARM:") && !tasks[i].completed) {
      char timeStr[10];
      sprintf(timeStr, "%02d:%02d", tasks[i].hour, tasks[i].minute);
      u8g2.drawStr(70, 42, timeStr);
      foundNext = true;
      break;
    }
  }
  
  if(!foundNext) {
    u8g2.drawStr(70, 42, "None");
  }
  
  // Instructions
  u8g2.drawStr(5, 54, "Tap: Test Sound");
  u8g2.drawStr(5, 66, "2Tap: Web Setup");
  
  u8g2.sendBuffer();
}

void handleAlarmTouch() {
  static unsigned long lastAlarmTouch = 0;
  static bool alarmTouchPressed = false;
  static int alarmTapCount = 0;
  static unsigned long alarmTapStart = 0;
  unsigned long currentTime = millis();
  
  bool currentTouch = (digitalRead(TOUCH_PIN_1) == HIGH);
  
  if(currentTouch && !alarmTouchPressed) {
    // Add debounce delay - only trigger if touch is stable for 50ms
    if(currentTime - lastAlarmTouch > 50) {
    alarmTouchPressed = true;
    alarmTapCount++;
    if(alarmTapCount == 1) {
        alarmTapStart = currentTime;
      }
    }
  } else if(!currentTouch && alarmTouchPressed) {
    alarmTouchPressed = false;
    
    // Check for single tap (within 300ms)
    if(alarmTapCount == 1 && currentTime - alarmTapStart < 300) {
      // Single tap - Test alarm sound
      testAlarmSound();
      alarmTapCount = 0;
    } else if(alarmTapCount == 2 && currentTime - alarmTapStart < 300) {
      // Double tap - Show web interface info
      showWebInterfaceInfo();
      alarmTapCount = 0;
    } else {
      // Reset tap count if too slow
      alarmTapCount = 0;
    }
    lastAlarmTouch = currentTime; // Update last touch time on release
  }
  
  // Reset tap count if too much time has passed
  if(currentTime - alarmTapStart > 300) {
    alarmTapCount = 0;
  }
}

void testAlarmSound() {
  // Play alarm sound sequence
  for(int i = 0; i < 3; i++) {
    playNotificationSound();
    delay(500);
  }
}

void showWebInterfaceInfo() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(5, 15, "Web Interface");
  u8g2.drawStr(5, 27, "Open browser to:");
  
  String ipStr = WiFi.localIP().toString();
  u8g2.drawStr(5, 39, ipStr.c_str());
  
  u8g2.drawStr(5, 51, "Use Alarm section");
  u8g2.drawStr(5, 63, "to set alarms");
  
  u8g2.sendBuffer();
  
  delay(3000); // Show for 3 seconds
}

void handleSettingsTouch() {
  static unsigned long lastSettingsTouch = 0;
  static bool settingsTouchPressed = false;
  unsigned long currentTime = millis();
  
  bool currentTouch = (digitalRead(TOUCH_PIN_1) == HIGH);
  
  if(currentTouch && !settingsTouchPressed) {
    // Add debounce delay - only trigger if touch is stable for 50ms
    if(currentTime - lastSettingsTouch > 50) {
      settingsTouchPressed = true;
      lastSettingsTouch = currentTime;
      
      // Return to menu
      currentState = STATE_MENU;
      Serial.println("Returning to menu from settings screen");
    }
  }
  
  if(!currentTouch && settingsTouchPressed) {
    settingsTouchPressed = false;
    lastSettingsTouch = currentTime; // Update last touch time on release
  }
}

void drawHeartAnimation() {
  // Heart animation positioned below BPM display
  static int heartFrame = 0;
  heartFrame = (heartFrame + 1) % 4;
  
  u8g2.setFont(u8g2_font_ncenB10_tr);
  
  int centerX = 50; // Center position
  int centerY = 45; // Below BPM display
  
  switch(heartFrame) {
    case 0:
      u8g2.drawStr(centerX, centerY, "<3");
      break;
    case 1:
      u8g2.drawStr(centerX, centerY, "♥");
      break;
    case 2:
      u8g2.drawStr(centerX, centerY, "<3");
      break;
    case 3:
      u8g2.drawStr(centerX, centerY, "♥");
      break;
  }
}
