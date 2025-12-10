/*
 * Configuration File for ESP32 Gemini Assistant
 * 
 * This file contains all the configurable parameters for the project.
 * Modify these values to customize the behavior of your assistant.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// HARDWARE PIN CONFIGURATION
// ============================================================================

// OLED Display (I2C)
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C

// Touch Sensor (TTP223) - Two sensors for menu navigation
#define TOUCH_PIN_1 4    // Touch Sensor 1: Open menu and navigate
#define TOUCH_PIN_2 5    // Touch Sensor 2: Move down list, cycle to top

// Heartbeat Sensor (Analog)
#define HEARTBEAT_PIN 34
#define HEARTBEAT_POWER_PIN 32  // GPIO pin to control heartbeat sensor power

// LED Indicator
#define LED_PIN 25

// Passive Buzzer
#define BUZZER_PIN 26

// ============================================================================
// DISPLAY CONFIGURATION
// ============================================================================

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Text Display Settings
#define TEXT_SIZE_SMALL 1
#define TEXT_SIZE_MEDIUM 2
#define TEXT_SIZE_LARGE 3

// ============================================================================
// TOUCH SENSOR CONFIGURATION
// ============================================================================

// Touch Detection Timing (milliseconds)
#define TOUCH_DEBOUNCE_MS 50
#define DOUBLE_TAP_MAX_MS 500
#define TRIPLE_TAP_MAX_MS 800

// Touch Sensitivity
#define TOUCH_THRESHOLD_HIGH 1
#define TOUCH_THRESHOLD_LOW 0

// ============================================================================
// HEARTBEAT SENSOR CONFIGURATION
// ============================================================================

// Sensor Buffer Settings
#define HEARTBEAT_BUFFER_SIZE 10
#define HEARTBEAT_SAMPLES 50
#define HEARTBEAT_THRESHOLD 100
#define BPM_CALCULATION_WINDOW 10

// Valid Heart Rate Range (BPM)
#define MIN_BPM 40
#define MAX_BPM 200

// Peak Detection Settings
#define PEAK_DETECTION_SAMPLES 3
#define MIN_PEAK_INTERVAL_MS 300
#define MAX_PEAK_INTERVAL_MS 2000

// ============================================================================
// ANIMATION CONFIGURATION
// ============================================================================

// Animation Timing (milliseconds)
#define ANIMATION_FRAME_DELAY 200
#define IDLE_BLINK_INTERVAL 3000
#define HEART_ANIMATION_DURATION 2000

// Buzzer Configuration
#define BUZZER_FREQUENCY_HIGH 2000
#define BUZZER_FREQUENCY_MEDIUM 1500
#define BUZZER_FREQUENCY_LOW 1000
#define BUZZER_DURATION_SHORT 200
#define BUZZER_DURATION_MEDIUM 500
#define BUZZER_DURATION_LONG 1000

// Animation Frame Dimensions
#define ANIM_WIDTH 32
#define ANIM_HEIGHT 32

// Animation States
#define ANIM_IDLE 0
#define ANIM_BLINK 1
#define ANIM_HAPPY 2
#define ANIM_HEART 3
#define ANIM_LOOK_LEFT 4
#define ANIM_LOOK_RIGHT 5
#define ANIM_SAD 6
#define ANIM_ANGRY 7
#define ANIM_CUTE 8
#define ANIM_WONDER 9

// ============================================================================
// TASK MANAGEMENT CONFIGURATION
// ============================================================================

// Task Storage Settings
#define MAX_TASKS 10
#define TASK_CHECK_INTERVAL_MS 60000  // Check every minute

// Task File Settings
#define TASKS_FILENAME "/tasks.json"
#define MAX_TASK_TITLE_LENGTH 50
#define MAX_TASK_DESC_LENGTH 100

// ============================================================================
// NETWORK CONFIGURATION
// ============================================================================

// WiFi Settings (Update these with your credentials)
#define WIFI_SSID "Vin-ByteLite(2.4G)"
#define WIFI_PASSWORD "@shovin12*(-_-)*"

// Web Server Settings
#define WEB_SERVER_PORT 80
#define MAX_CONNECTIONS 4

// Connection Timeouts (milliseconds)
#define WIFI_CONNECT_TIMEOUT 10000
#define HTTP_TIMEOUT 5000

// ============================================================================
// GEMINI API CONFIGURATION
// ============================================================================

// API Settings
#define GEMINI_API_KEY "AIzaSyBy8NERIqaFiQu84x8_Wc4Vqq1pztBLbhU"
#define GEMINI_ENDPOINT "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateConten"

// Request Settings
#define MAX_REQUEST_LENGTH 1000
#define MAX_RESPONSE_LENGTH 2000
#define API_REQUEST_TIMEOUT 10000

// ============================================================================
// SYSTEM CONFIGURATION
// ============================================================================

// Serial Communication
#define SERIAL_BAUD_RATE 115200

// Memory Management
#define JSON_BUFFER_SIZE 1024
#define MAX_STRING_LENGTH 256

// System Timing
#define MAIN_LOOP_DELAY_MS 10
#define DISPLAY_UPDATE_INTERVAL_MS 100

// ============================================================================
// DEBUG CONFIGURATION
// ============================================================================

// Debug Settings
#define DEBUG_MODE true
#define DEBUG_TOUCH false
#define DEBUG_HEARTBEAT false
#define DEBUG_ANIMATIONS false
#define DEBUG_TASKS false
#define DEBUG_NETWORK false
#define DEBUG_GEMINI false

// Debug Macros
#if DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#if DEBUG_TOUCH
  #define DEBUG_TOUCH_PRINT(x) Serial.print(x)
  #define DEBUG_TOUCH_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_TOUCH_PRINT(x)
  #define DEBUG_TOUCH_PRINTLN(x)
#endif

#if DEBUG_HEARTBEAT
  #define DEBUG_HEARTBEAT_PRINT(x) Serial.print(x)
  #define DEBUG_HEARTBEAT_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_HEARTBEAT_PRINT(x)
  #define DEBUG_HEARTBEAT_PRINTLN(x)
#endif

#if DEBUG_ANIMATIONS
  #define DEBUG_ANIM_PRINT(x) Serial.print(x)
  #define DEBUG_ANIM_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_ANIM_PRINT(x)
  #define DEBUG_ANIM_PRINTLN(x)
#endif

#if DEBUG_TASKS
  #define DEBUG_TASK_PRINT(x) Serial.print(x)
  #define DEBUG_TASK_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_TASK_PRINT(x)
  #define DEBUG_TASK_PRINTLN(x)
#endif

#if DEBUG_NETWORK
  #define DEBUG_NET_PRINT(x) Serial.print(x)
  #define DEBUG_NET_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_NET_PRINT(x)
  #define DEBUG_NET_PRINTLN(x)
#endif

#if DEBUG_GEMINI
  #define DEBUG_GEMINI_PRINT(x) Serial.print(x)
  #define DEBUG_GEMINI_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_GEMINI_PRINT(x)
  #define DEBUG_GEMINI_PRINTLN(x)
#endif

// ============================================================================
// SYSTEM STATES
// ============================================================================

enum SystemState {
  STATE_IDLE,
  STATE_MENU,
  STATE_TASK_DISPLAY,
  STATE_HEARTBEAT_MONITOR,
  STATE_GEMINI_CHAT,
  STATE_NOTIFICATION,
  STATE_LAMP_CONTROL,
  STATE_ALARM_CONTROL,
  STATE_SETTINGS
};

// ============================================================================
// ERROR CODES
// ============================================================================

enum ErrorCode {
  ERROR_NONE = 0,
  ERROR_WIFI_CONNECTION_FAILED,
  ERROR_SPIFFS_INIT_FAILED,
  ERROR_OLED_INIT_FAILED,
  ERROR_GEMINI_API_FAILED,
  ERROR_TASK_SAVE_FAILED,
  ERROR_TASK_LOAD_FAILED,
  ERROR_MEMORY_ALLOCATION_FAILED
};

// ============================================================================
// UTILITY MACROS
// ============================================================================

// Array size calculation
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// Min/Max macros
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Clamp value between min and max
#define CLAMP(value, min_val, max_val) ((value) < (min_val) ? (min_val) : ((value) > (max_val) ? (max_val) : (value)))

// ============================================================================
// VERSION INFORMATION
// ============================================================================

#define FIRMWARE_VERSION "1.0.0"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

#endif // CONFIG_H
