#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "../config/config.h"

class BootGuard {
public:
  void begin();
  bool shouldResetWiFi();
  void clear();

private:
  Preferences prefs;
  int count = 0;
  unsigned long bootTime = 0;
};
