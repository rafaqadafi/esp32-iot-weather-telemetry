#include "boot.h"

void BootGuard::begin() {
  prefs.begin("boot", false);
  count = prefs.getInt("count", 0);
  bootTime = prefs.getULong("time", 0);
  unsigned long nowBoot = millis();
  if (nowBoot - bootTime > BOOT_WINDOW_MS) count = 0;
  count++;
  prefs.putInt("count", count);
  prefs.putULong("time", nowBoot);
  prefs.end();
  Serial.printf("[BOOT] Boot count: %d\n", count);
}

bool BootGuard::shouldResetWiFi() {
  return count >= 3;
}

void BootGuard::clear() {
  prefs.begin("boot", false);
  prefs.putInt("count", 0);
  prefs.end();
}
