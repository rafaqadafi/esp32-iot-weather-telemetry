#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../config/config.h"

class ThingSpeakUpload {
public:
  void send(float temperature, float humidity, float lux);
};
