#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "../config/config.h"

class MqttStack {
public:
  void begin();
  void send(float temperature, float humidity);

private:
  bool ensureConnected();
  void publishDiscovery();
  WiFiClient espClient;
  PubSubClient client{espClient};
};
