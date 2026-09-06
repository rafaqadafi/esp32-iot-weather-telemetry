#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "../config/config.h"

class OledView {
public:
  bool begin();
  void setSensors(float temperature, float humidity, float lux);
  void setWiFi(bool connected);
  void setUploading(bool uploading);
  void update();
  void showBoot();
  void showWiFiFail();
  void showWifiSetup();
  void showResetWifi();

private:
  void drawStatusIcons();

  Adafruit_SSD1306 display{OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET_PIN};
  float temperature = 0.0;
  float humidity = 0.0;
  float lux = 0.0;
  bool wifiConnected = false;
  bool isUploading = false;
};
