#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include <FastLED.h>
#include "config.h"

struct Settings {
  int brightness;
  CRGB currentColor;
  int effectIndex;
  bool invertDisplay;
  bool rotateDisplay180;
};

extern Settings settings;

void loadSettings(Settings &settings);
void saveSettings(const Settings &settings);

#endif