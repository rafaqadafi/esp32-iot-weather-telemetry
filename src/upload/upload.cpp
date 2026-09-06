#include "upload.h"

void ThingSpeakUpload::send(float temperature, float humidity, float lux) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClient client;
  HTTPClient http;
  String url = String(THINGSPEAK_SERVER) +
               "?api_key=" + THINGSPEAK_API_KEY +
               "&field1=" + String(temperature, 2) +
               "&field2=" + String(humidity, 2) +
               "&field3=" + String(lux, 1);

  http.begin(client, url);
  http.setTimeout(10000);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String response = http.getString();
    Serial.print("[ThingSpeak] Upload OK, entry: ");
    Serial.println(response);
  } else {
    Serial.print("[ThingSpeak] Upload gagal, error: ");
    Serial.println(http.errorToString(httpCode));
  }
  http.end();
}
