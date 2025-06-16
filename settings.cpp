#include "config.h"
#include "display.h"
#include "leds.h"
#include "game.h"
#include "settings.h"
#include "state.h"
#include <Arduino.h>
#include <EEPROM.h>


void loadSettings(Settings &settings) {
  settings.brightness = EEPROM.read(EEPROM_BRIGHTNESS);
  if (settings.brightness < 16 || settings.brightness > 255) settings.brightness = 16;
  
  settings.currentColor.r = EEPROM.read(EEPROM_RED);
  settings.currentColor.g = EEPROM.read(EEPROM_GREEN);
  settings.currentColor.b = EEPROM.read(EEPROM_BLUE);
  
  settings.effectIndex = EEPROM.read(EEPROM_EFFECT);
  if (settings.effectIndex >= EFFECT_COUNT) settings.effectIndex = 0;
}

void saveSettings(const Settings &settings) {
  EEPROM.update(EEPROM_BRIGHTNESS, settings.brightness);
  EEPROM.update(EEPROM_RED, settings.currentColor.r);
  EEPROM.update(EEPROM_GREEN, settings.currentColor.g);
  EEPROM.update(EEPROM_BLUE, settings.currentColor.b);
  EEPROM.update(EEPROM_EFFECT, settings.effectIndex);
}
