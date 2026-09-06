#pragma once

// ===================== PIN CONFIGURATION =====================
#define PIN_RXD2            17
#define PIN_TXD2            4
#define PIN_I2C_SDA         18
#define PIN_I2C_SCL         22

// ===================== MODBUS CONFIGURATION ==================
#define MODBUS_BAUD         9600
#define MODBUS_SLAVE_ID     1

// ===================== TIMING INTERVALS (ms) =================
#define READ_INTERVAL_MS    200
#define MQTT_INTERVAL_MS    5000
#define UPLOAD_INTERVAL_MS  15000
#define BOOT_WINDOW_MS      10000

// ===================== OLED CONFIGURATION ====================
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_RESET_PIN      -1
#define OLED_I2C_ADDR       0x3C

// ===================== SENSOR CONFIGURATION ==================
#define BH1750_ADDR_PRIMARY   0x23
#define BH1750_ADDR_SECONDARY 0x5C

// ===================== CREDENTIALS ============================
#if __has_include("credentials.h")
#include "credentials.h"
#else
#include "credentials.h.example"
#endif

// ===================== THINGSPEAK CONFIGURATION ==============
#define THINGSPEAK_SERVER   "http://api.thingspeak.com/update"

// ===================== WIFI MANAGER ==========================
#define WIFI_AP_NAME        "ESP32-Config"
#define WIFI_AP_PASS        "12345678"
#define WIFI_PORTAL_TIMEOUT 180
#define WIFI_CONN_TIMEOUT   30

// ===================== MQTT CONFIGURATION =====================
#define MQTT_SERVER         "broker.emqx.io"
#define MQTT_PORT           1883
#define MQTT_TOPIC          "ABT-SuhuKelembapan/data"
