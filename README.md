# esp32-iot-weather-telemetry

Firmware ESP32 real-time environmental monitoring system berbasis **FreeRTOS multi-core multitasking**.

## Features
- **Sensor Suhu & Kelembaban**: Modbus RTU (XY-MD02) via Hardware Serial UART2.
- **Sensor Cahaya**: BH1750 Ambient Light Sensor via I2C.
- **Display Status**: OLED SSD1306 128x64 via I2C (4 FPS refresh rate).
- **WiFi Management**: WiFiManager dengan Captive Portal fallback AP (`ESP32-Config`).
- **Telemetry Cloud & IoT**:
  - **ThingSpeak HTTP API**: Upload otomatis tiap 15 detik (Field1: Suhu, Field2: Kelembaban, Field3: Lux).
  - **MQTT Telemetry**: Kirim JSON tiap 5 detik ke broker (`broker.emqx.io:1883`, topic: `ABT-SuhuKelembapan/data`).
  - **Home Assistant Auto-Discovery**: Integrasi otomatis tanpa perlu konfigurasi YAML manual di HA.
- **Boot Protection**: Auto-wipe WiFi credentials jika terdeteksi 3x quick reboot dalam 10 detik.

---

## Hardware Pinout

| Komponen | Protokol / Jalur | Pin ESP32 | Keterangan |
|---|---|---|---|
| **Modbus MD02** | UART2 (Serial2) | RX2: `GPIO 17`<br>TX2: `GPIO 4` | Baud 9600, 8N1, Slave ID 1 |
| **BH1750** | I2C (Wire) | SDA: `GPIO 18`<br>SCL: `GPIO 22` | Addr: `0x23` (fallback `0x5C`) |
| **OLED SSD1306** | I2C (Wire) | SDA: `GPIO 18`<br>SCL: `GPIO 22` | Addr: `0x3C`, 128x64 px |

---

## FreeRTOS Multi-Core Architecture

- **`taskSensor`** (Core 1, Pri 3, 200 ms): Deterministic Modbus RTU & BH1750 sampling.
- **`taskDisplay`** (Core 1, Pri 2, 250 ms): 4 FPS OLED UI rendering with status icons.
- **`taskNetwork`** (Core 0, Pri 1, 5000 ms loop): Non-blocking WiFi reconnect check, MQTT publish (5s), dan ThingSpeak HTTP upload (15s).
- **Concurrency & Mutex**:
  - `gI2cMutex`: Proteksi bus I2C bersama antara BH1750 dan OLED SSD1306.
  - `gDataMutex`: Proteksi thread-safe data snapshot `SensorData`.

---

## Build & Flash (PlatformIO)

```bash
# Compile firmware
pio run

# Flash ke ESP32
pio run -t upload

# Buka Serial Monitor (115200 baud)
pio device monitor -b 115200
```
