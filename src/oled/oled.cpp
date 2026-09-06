#include "oled.h"

static const unsigned char PROGMEM icon_wifi[] = {
  0b00011111, 0b10000000,
  0b01100000, 0b01000000,
  0b10000000, 0b00100000,
  0b01111111, 0b01000000,
  0b00000000, 0b10000000,
  0b00011111, 0b00000000,
  0b00100000, 0b10000000,
  0b00011100, 0b00000000,
  0b00001000, 0b00000000,
  0b00001000, 0b00000000,
};

static const unsigned char PROGMEM icon_upload[] = {
  0b00010000,
  0b00111000,
  0b01111100,
  0b11111110,
  0b00010000,
  0b00010000,
  0b11111110,
  0b11111110,
  0b00000000,
  0b00000000,
};

bool OledView::begin() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) return false;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 25);
  display.print("Initializing...");
  display.display();
  return true;
}

void OledView::setSensors(float t, float h, float l) {
  temperature = t;
  humidity = h;
  lux = l;
}

void OledView::setWiFi(bool connected) {
  wifiConnected = connected;
}

void OledView::setUploading(bool uploading) {
  isUploading = uploading;
}

void OledView::drawStatusIcons() {
  if (wifiConnected) display.drawBitmap(104, 0, icon_wifi, 13, 10, SSD1306_WHITE);
  else {
    display.setCursor(104, 0);
    display.setTextSize(1);
    display.print("X");
  }
  if (isUploading) display.drawBitmap(118, 0, icon_upload, 8, 10, SSD1306_WHITE);
}

void OledView::update() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("MONITORING");
  drawStatusIcons();
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);
  display.drawFastVLine(63, 10, 40, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 13);
  display.print("Suhu");
  display.setTextSize(2);
  display.setCursor(0, 23);
  display.print(temperature, 1);
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(68, 13);
  display.print("Lux");
  display.setTextSize(2);
  display.setCursor(68, 23);
  display.print((int)lux);
  display.setTextSize(1);
  display.setCursor(68, 40);
  display.print("lx");

  display.drawFastHLine(0, 50, 128, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 54);
  display.print("RH: ");
  display.print(humidity, 1);
  display.print(" %");
  display.setCursor(70, 54);
  if (!wifiConnected) display.print("No WiFi");
  else if (isUploading) display.print("Upload..");
  else display.print("WiFi OK");
  display.display();
}

void OledView::showBoot() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 25);
  display.print("Initializing...");
  display.display();
}

void OledView::showWiFiFail() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("WiFi Gagal!");
  display.setCursor(0, 35);
  display.print("Restart...");
  display.display();
}

void OledView::showWifiSetup() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("WiFi Setup:");
  display.setCursor(0, 15);
  display.print("Hotspot:");
  display.setCursor(0, 25);
  display.print(WIFI_AP_NAME);
  display.setCursor(0, 40);
  display.print("Pass: ");
  display.print(WIFI_AP_PASS);
  display.setCursor(0, 54);
  display.print("192.168.4.1");
  display.display();
}

void OledView::showResetWifi() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("Reset WiFi...");
  display.setCursor(0, 25);
  display.print("Sambung hotspot:");
  display.setCursor(0, 35);
  display.print(WIFI_AP_NAME);
  display.setCursor(0, 45);
  display.print("Pass: ");
  display.print(WIFI_AP_PASS);
  display.display();
}
