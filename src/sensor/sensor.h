#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <ModbusMaster.h>
#include <BH1750.h>
#include "../config/config.h"

class SensorStack {
public:
  void begin();
  void read();
  float temperature() const;
  float humidity() const;
  float lux() const;

private:
  BH1750 lightMeter;
  ModbusMaster node;
  float t = 0.0;
  float h = 0.0;
  float l = 0.0;
};
