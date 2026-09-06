#include "sensor.h"

void SensorStack::begin() {
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, BH1750_ADDR_PRIMARY, &Wire)) {
    Serial.println("BH1750 gagal init, coba 0x5C...");
    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, BH1750_ADDR_SECONDARY, &Wire)) {
      Serial.println("BH1750 tetap gagal!");
    }
  } else {
    Serial.println("BH1750 OK");
  }
  delay(200);
  Serial2.begin(MODBUS_BAUD, SERIAL_8N1, PIN_RXD2, PIN_TXD2);
  node.begin(MODBUS_SLAVE_ID, Serial2);
}

void SensorStack::read() {
  uint8_t result = node.readInputRegisters(0x0001, 2);
  if (result == node.ku8MBSuccess) {
    t = node.getResponseBuffer(0) / 100.0;
    h = node.getResponseBuffer(1) / 100.0;
  } else {
    Serial.print("Modbus Error: 0x");
    Serial.println(result, HEX);
  }
  if (lightMeter.measurementReady()) l = lightMeter.readLightLevel();
}

float SensorStack::temperature() const { return t; }
float SensorStack::humidity() const { return h; }
float SensorStack::lux() const { return l; }
