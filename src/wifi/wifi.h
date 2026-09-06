#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "../oled/oled.h"

class WiFiStack {
public:
  void connect(OledView& oled);
  bool connected() const;
  bool reconnect();

private:
  bool wifiConnected = false;
};
