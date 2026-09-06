#include "mqtt.h"

void MqttStack::begin() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setBufferSize(512);
}

void MqttStack::publishDiscovery() {
  const char* topicTemp = "homeassistant/sensor/abt_suhu/config";
  const char* payloadTemp = "{\"name\":\"Suhu\",\"state_topic\":\"" MQTT_TOPIC "\",\"value_template\":\"{{ value_json.suhu }}\",\"unit_of_measurement\":\"°C\",\"device_class\":\"temperature\",\"state_class\":\"measurement\",\"unique_id\":\"abt_suhu\",\"device\":{\"identifiers\":[\"abt_esp32\"],\"name\":\"ABT Monitor\",\"model\":\"ESP32 MD02 BH1750\",\"manufacturer\":\"ABT\"}}";

  const char* topicHum = "homeassistant/sensor/abt_kelembapan/config";
  const char* payloadHum = "{\"name\":\"Kelembapan\",\"state_topic\":\"" MQTT_TOPIC "\",\"value_template\":\"{{ value_json.kelembapan }}\",\"unit_of_measurement\":\"%\",\"device_class\":\"humidity\",\"state_class\":\"measurement\",\"unique_id\":\"abt_kelembapan\",\"device\":{\"identifiers\":[\"abt_esp32\"],\"name\":\"ABT Monitor\",\"model\":\"ESP32 MD02 BH1750\",\"manufacturer\":\"ABT\"}}";

  client.publish(topicTemp, payloadTemp, true);
  client.publish(topicHum, payloadHum, true);
  Serial.println("[MQTT] Home Assistant discovery sent (retained).");
}

bool MqttStack::ensureConnected() {
  if (client.connected()) return true;
  if (WiFi.status() != WL_CONNECTED) return false;

  String clientId = "ESP32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  Serial.print("[MQTT] Menghubungkan ke ");
  Serial.println(MQTT_SERVER);

  if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
    Serial.println("[MQTT] Terhubung!");
    publishDiscovery();
    return true;
  } else {
    Serial.print("[MQTT] Gagal rc=");
    Serial.println(client.state());
    return false;
  }
}

void MqttStack::send(float temperature, float humidity) {
  if (!ensureConnected()) return;

  char payload[64];
  snprintf(payload, sizeof(payload), "{\"suhu\":%.2f,\"kelembapan\":%.2f}", temperature, humidity);

  Serial.print("[MQTT] Publish ke ");
  Serial.print(MQTT_TOPIC);
  Serial.print(" : ");
  Serial.println(payload);

  client.publish(MQTT_TOPIC, payload);
  client.loop();
}
