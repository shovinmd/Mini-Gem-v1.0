#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h> // optional if you disable WiFi during updates

#define SDA_PIN 21
#define SCL_PIN 22
#define OLED_ADDR 0x3D      // your device
#define FRAME_GUARD_MS 50   // min delay between frames

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clk=*/ SCL_PIN, /*data=*/ SDA_PIN, U8X8_PIN_NONE);


void ssd1306_soft_init_via_wire() {
  // send a few init commands in case the device needs it (safe re-init)
  const uint8_t cmds[] = {
    0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
    0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
    0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
  };
  for (size_t i = 0; i < sizeof(cmds); ++i) {
    Wire.beginTransmission(OLED_ADDR);
    Wire.write(0x00); // command
    Wire.write(cmds[i]);
    Wire.endTransmission();
    delay(5);
  }
}

bool i2c_device_present() {
  Wire.beginTransmission(OLED_ADDR);
  uint8_t err = Wire.endTransmission();
  return (err == 0);
}

void safe_display_sendbuffer() {
  // quick check — is the device responding now?
  if (!i2c_device_present()) {
    Serial.println("OLED: NACK before sending buffer -> skipping frame and trying re-init");
    ssd1306_soft_init_via_wire(); // attempt soft re-init
    delay(50);
    if (!i2c_device_present()) {
      Serial.println("OLED: still NACK after re-init -> aborting frame");
      return; // skip this frame to avoid flooding bus
    }
  }

  // optionally disable WiFi briefly (uncomment if you see power/brownout issues)
  // wifi will be re-enabled after frame by user if desired
  bool wifiWasOn = false;
  #if defined(WiFi_h)
  if (WiFi.status() == WL_CONNECTED) {
    wifiWasOn = true;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(10);
  }
  #endif

  u8g2.sendBuffer(); // the potentially-big transfer

  #if defined(WiFi_h)
  if (wifiWasOn) {
    // you can re-enable WiFi here if needed (but better re-connect in main flow)
    WiFi.mode(WIFI_STA);
    // WiFi.begin(ssid, pass); // if you want auto reconnect (optional)
  }
  #endif

  delay(FRAME_GUARD_MS); // short guard so transfers are spaced out
}

void setup() {
  Serial.begin(115200);
  delay(50);

  // init I2C slower and stable
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(80000); // slower than 100k — try 80k or 50k if issues persist
  delay(20);

  u8g2.setI2CAddress(OLED_ADDR); // tell u8g2 the address before begin()
  if (!u8g2.begin()) {
    Serial.println("u8g2.begin() failed - trying soft init via Wire");
    ssd1306_soft_init_via_wire();
    if (!u8g2.begin()) {
      Serial.println("u8g2.begin() still failed - continue, but display might be unstable");
    } else Serial.println("u8g2.begin() OK after soft init");
  } else Serial.println("u8g2.begin() OK");

  // optional: set contrast (helps with black/white rendering)
  Wire.beginTransmission(OLED_ADDR); Wire.write(0x00); Wire.write(0x81); Wire.write(0xCF); Wire.endTransmission();

  // draw initial buffer
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0,12,"Display init OK");
  u8g2.sendBuffer();
}

void loop() {
  // Example dynamic drawing with safe send
  static int r = 0;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0,10,"U8g2 Safe Frame");
  u8g2.drawCircle(64,32,abs((r%40)-20));
  // instead of u8g2.sendBuffer() call our safe wrapper
  safe_display_sendbuffer();
  r++;
  // do other sensor work here; ensure sensor reads do not interrupt I2C transfer
  delay(50);
}
