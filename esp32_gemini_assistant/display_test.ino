/*
 * OLED Display Test Sketch
 * Tests the SSD1306 OLED display functionality
 *
 * Hardware:
 * - OLED Display (I2C): SDA=GPIO21, SCL=GPIO22, Address=0x3C
 */

#include <Wire.h>
#include <U8g2lib.h>

// OLED Display Configuration
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C

// Initialize OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  Serial.println("=== OLED Display Test ===");

  // Initialize I2C
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  // Initialize OLED display
  if(u8g2.begin()) {
    Serial.println("OLED display initialized successfully!");
  } else {
    Serial.println("Failed to initialize OLED display!");
    while(1); // Halt if display fails
  }

  // Clear display
  u8g2.clearBuffer();

  // Display test message
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(10, 20, "Display Test");
  u8g2.drawStr(10, 40, "OLED Working!");
  u8g2.drawStr(10, 60, "ESP32 Ready");

  // Send buffer to display
  u8g2.sendBuffer();

  Serial.println("Test message displayed on OLED");
  Serial.println("If you can see the text on the display, it's working!");
}

void loop() {
  // Simple animation test
  static int frame = 0;
  frame++;

  u8g2.clearBuffer();

  // Display frame counter
  u8g2.setFont(u8g2_font_ncenB08_tr);
  char frameStr[20];
  sprintf(frameStr, "Frame: %d", frame);
  u8g2.drawStr(10, 15, frameStr);

  // Display some test patterns
  u8g2.drawCircle(64, 32, 20 + (frame % 10), U8G2_DRAW_ALL);
  u8g2.drawStr(10, 50, "Testing...");

  u8g2.sendBuffer();

  delay(500); // Update every 500ms

  // Reset frame counter after 100 frames
  if(frame >= 100) {
    frame = 0;
  }
}
