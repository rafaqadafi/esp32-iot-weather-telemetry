#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "config/config.h"
#include "oled/oled.h"
#include "sensor/sensor.h"
#include "boot/boot.h"
#include "wifi/wifi.h"
#include "upload/upload.h"
#include "mqtt/mqtt.h"

// ==========================================
// SHARED DATA & SYNCHRONIZATION
// ==========================================
struct SensorData {
  float temperature;
  float humidity;
  float lux;
  bool isUploading;
  bool wifiConnected;
};

static SensorData gData = {0.0f, 0.0f, 0.0f, false, false};
static SemaphoreHandle_t gDataMutex = NULL;
static SemaphoreHandle_t gI2cMutex = NULL;

// Hardware drivers
static OledView oled;
static SensorStack sensors;
static BootGuard bootGuard;
static WiFiStack wifiStack;
static ThingSpeakUpload uploader;
static MqttStack mqtt;

// ==========================================
// TASK PRIORITIES & CORE ASSIGNMENTS
// Priority Rules:
// 1. TaskSensor (Pri 3, Core 1) -> Real-time periodic reading, Modbus timing sensitive.
// 2. TaskDisplay (Pri 2, Core 1) -> Smooth UI update, shares I2C with BH1750.
// 3. TaskNetwork (Pri 1, Core 0) -> High latency / blocking operations (WiFi & HTTP).
// ==========================================

void taskSensor(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(READ_INTERVAL_MS);

  for (;;) {
    // 1. Baca sensor (I2C dilindungi mutex agar aman saat bersamaan dgn OLED)
    if (xSemaphoreTake(gI2cMutex, portMAX_DELAY) == pdTRUE) {
      sensors.read();
      xSemaphoreGive(gI2cMutex);
    }

    // 2. Update shared data
    if (xSemaphoreTake(gDataMutex, portMAX_DELAY) == pdTRUE) {
      gData.temperature = sensors.temperature();
      gData.humidity = sensors.humidity();
      gData.lux = sensors.lux();
      xSemaphoreGive(gDataMutex);
    }

    Serial.println("─────────────────────────");
    Serial.print("Suhu       : "); Serial.print(sensors.temperature(), 2); Serial.println(" °C");
    Serial.print("Kelembapan : "); Serial.print(sensors.humidity(), 2);    Serial.println(" %RH");
    Serial.print("Cahaya     : "); Serial.print(sensors.lux(), 1);         Serial.println(" lx");

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void taskDisplay(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(250); // Refresh UI 4x per detik

  SensorData local;
  for (;;) {
    // Ambil data snapshot terbaru
    if (xSemaphoreTake(gDataMutex, portMAX_DELAY) == pdTRUE) {
      local = gData;
      xSemaphoreGive(gDataMutex);
    }

    // Update tampilan OLED dengan proteksi I2C Mutex
    if (xSemaphoreTake(gI2cMutex, portMAX_DELAY) == pdTRUE) {
      oled.setSensors(local.temperature, local.humidity, local.lux);
      oled.setWiFi(local.wifiConnected);
      oled.setUploading(local.isUploading);
      oled.update();
      xSemaphoreGive(gI2cMutex);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void taskNetwork(void *pvParameters) {
  SensorData local;
  unsigned long lastThingSpeak = 0;

  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(MQTT_INTERVAL_MS));

    // Cek koneksi WiFi
    if (WiFi.status() != WL_CONNECTED) {
      if (xSemaphoreTake(gDataMutex, portMAX_DELAY) == pdTRUE) {
        gData.wifiConnected = false;
        xSemaphoreGive(gDataMutex);
      }
      wifiStack.reconnect();
    }

    bool isConnected = (WiFi.status() == WL_CONNECTED);
    if (xSemaphoreTake(gDataMutex, portMAX_DELAY) == pdTRUE) {
      gData.wifiConnected = isConnected;
      xSemaphoreGive(gDataMutex);
    }

    if (!isConnected) {
      continue;
    }

    if (xSemaphoreTake(gDataMutex, portMAX_DELAY) == pdTRUE) {
      gData.isUploading = true;
      local = gData;
      xSemaphoreGive(gDataMutex);
    }

    // 1. MQTT kirim setiap 5 detik
    mqtt.send(local.temperature, local.humidity);

    // 2. ThingSpeak kirim setiap 15 detik
    unsigned long now = millis();
    if (now - lastThingSpeak >= UPLOAD_INTERVAL_MS) {
      lastThingSpeak = now;
      Serial.println("[ThingSpeak] Mengupload data...");
      uploader.send(local.temperature, local.humidity, local.lux);
    }

    if (xSemaphoreTake(gDataMutex, portMAX_DELAY) == pdTRUE) {
      gData.isUploading = false;
      xSemaphoreGive(gDataMutex);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Inisialisasi Mutex
  gDataMutex = xSemaphoreCreateMutex();
  gI2cMutex = xSemaphoreCreateMutex();

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  if (!oled.begin()) {
    Serial.println("OLED tidak ditemukan!");
  } else {
    Serial.println("OLED OK");
    oled.showBoot();
  }

  bootGuard.begin();
  if (bootGuard.shouldResetWiFi()) {
    Serial.println("[WiFi] 3x boot terdeteksi, reset WiFi...");
    oled.showResetWifi();
    bootGuard.clear();
    WiFiManager wm;
    wm.resetSettings();
    delay(2000);
    ESP.restart();
  }

  Serial.print("Scan I2C... ");
  Wire.beginTransmission(BH1750_ADDR_PRIMARY);
  if (Wire.endTransmission() == 0) {
    Serial.println("BH1750 ditemukan di 0x23");
  } else {
    Wire.beginTransmission(BH1750_ADDR_SECONDARY);
    if (Wire.endTransmission() == 0) {
      Serial.println("BH1750 ditemukan di 0x5C");
    } else {
      Serial.println("BH1750 TIDAK ditemukan!");
    }
  }

  sensors.begin();
  mqtt.begin();

  // Koneksi WiFi awal saat setup
  wifiStack.connect(oled);
  gData.wifiConnected = wifiStack.connected();

  bootGuard.clear();
  Serial.println("=== FreeRTOS: MD02 + BH1750 + ThingSpeak + MQTT + OLED ===");

  // ==========================================
  // SPAWN FREERTOS TASKS
  // Core 1: Sensor & Display (Timing & UI)
  // Core 0: Network / WiFi / HTTP (Blocking I/O)
  // ==========================================
  xTaskCreatePinnedToCore(taskSensor,  "SensorTask",  4096, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(taskDisplay, "DisplayTask", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(taskNetwork, "NetworkTask", 8192, NULL, 1, NULL, 0);
}

void loop() {
  // Loop kosong: semua eksekusi dikelola scheduler FreeRTOS
  vTaskDelete(NULL);
}
