#include "wifi.h"

void WiFiStack::connect(OledView& oled) {
  Serial.println("Menghubungkan ke WiFi...");
  wifiConnected = false;
  oled.setWiFi(false);
  oled.update();

  WiFiManager wm;
  wm.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT);
  wm.setConnectTimeout(WIFI_CONN_TIMEOUT);
  wm.setAPCallback([&](WiFiManager*) { oled.showWifiSetup(); });

  if (!wm.autoConnect(WIFI_AP_NAME, WIFI_AP_PASS)) {
    Serial.println("Gagal konek, restart...");
    oled.showWiFiFail();
    delay(2000);
    ESP.restart();
  }

  wifiConnected = true;
  oled.setWiFi(true);
  Serial.println("[WiFi] Konek: " + WiFi.localIP().toString());
  oled.update();
}

bool WiFiStack::reconnect() {
  wifiConnected = false;
  Serial.println("[WiFi] Terputus, reconnect...");
  WiFi.reconnect();
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    attempt++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    return true;
  } else {
    Serial.println("[WiFi] Reconnect gagal, skip upload.");
    return false;
  }
}

bool WiFiStack::connected() const { return wifiConnected; }
