#include "config.h"
#include "display.h"
#include "leds.h"
#include "settings.h"
#include "state.h"
#include <Arduino.h>

CRGB leds[LED_COUNT];

void initLEDs() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(EEPROM.read(EEPROM_BRIGHTNESS));
}

void updateLEDs(int effectIndex, CRGB currentColor) {
  switch (effectIndex) {
    case 0: effectStatic(currentColor); break;
    case 1: effectRunningDot(currentColor); break;
    case 2: effectConfetti(); break;
    case 3: effectStaboscope(); break;
    case 4: effectSoftGlow(); break;
    case 5: effectCyberpunk(); break;
    case 6: effectHazard(); break;
    case 7: effectPolice(); break;
    default: effectStatic(currentColor); break;
  }
}

void effectStatic(CRGB color) {
  fill_solid(leds, LED_COUNT, color);
}

void effectRunningDot(CRGB color) {
  static int pos = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate > 50) {
    lastUpdate = millis();
    fadeToBlackBy(leds, LED_COUNT, 50);
    leds[pos] = color;
    pos = (pos + 1) % LED_COUNT;
  }
}

void effectConfetti() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 30) {
    lastUpdate = millis();
    fadeToBlackBy(leds, LED_COUNT, 10);
    int pos = random8() % LED_COUNT;
    leds[pos] = CHSV(random8(), 255, 255);
  }
}

void effectStaboscope() {
  static unsigned long lastFlash = 0;
  static int flashCount = 0;
  static bool flashing = false;

  if (!flashing) {
    if (millis() - lastFlash > 5000) {
      flashing = true;
      flashCount = 0;
    }
    fill_solid(leds, LED_COUNT, CRGB::Black);
  } else {
    if (millis() - lastFlash > 80) {
      if (flashCount % 2 == 0) {
        fill_solid(leds, LED_COUNT, CRGB(255, 255, 255));
        FastLED.setBrightness(255);
      } else {
        fill_solid(leds, LED_COUNT, CRGB::Black);
      }
      flashCount++;
      lastFlash = millis();

      if (flashCount >= 18) {
        flashing = false;
      }
    }
  }
}

// === Soft Glow ===
void effectSoftGlow() {
  static unsigned long lastUpdate = 0;
  static uint8_t phase = 0;

  if (millis() - lastUpdate > 40) {
    lastUpdate = millis();

    uint8_t glow = sin8(phase);
    fill_solid(leds, LED_COUNT, CHSV(32, 200, glow)); // теплий оранжево-жовтий
    phase += 2;

    FastLED.show();
  }
}

// === Cyberpunk Neon Wave ===
void effectCyberpunk() {
  static uint8_t wavePos = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate > 25) {
    lastUpdate = millis();

    for (int i = 0; i < LED_COUNT; i++) {
      uint8_t brightness = sin8(i * 12 + wavePos);
      uint8_t hue = 160 + sin8(i * 10 + wavePos) / 8; // 160-200: фіолетово-рожевий
      leds[i] = CHSV(hue, 255, brightness);
    }

    wavePos += 3;
    FastLED.show();
  }
}

void effectHazard() {
  static unsigned long lastToggle = 0;
  static bool state = false;

  if (millis() - lastToggle > 500) {
    lastToggle = millis();
    state = !state;
    CRGB color = state ? CRGB(255, 60, 0) : CRGB::Black;

    leds[0] = leds[9] = leds[4] = leds[5] = color;
    for (int i = 0; i < LED_COUNT; i++) {
      if (i != 0 && i != 4 && i != 5 && i != 9) {
        leds[i] = CRGB::Black;
      }
    }
  }
}

void effectPolice() {
  static unsigned long lastUpdate = 0;
  static int blinkCount = 0;
  static bool leftSide = true;
  static bool isOn = false;

  unsigned long now = millis();
  const int blinkInterval = 100;
  const int pauseBetweenSides = 300;

  if (now - lastUpdate >= blinkInterval) {
    lastUpdate = now;
    fill_solid(leds, LED_COUNT, CRGB::Black);

    if (isOn) {
      if (leftSide) {
        leds[0] = leds[1] = leds[9] = leds[8] = leds[7] = CRGB(0, 0, 255); // синій
      } else {
        leds[2] = leds[3] = leds[4] = leds[5] = leds[6] = CRGB(255, 0, 0); // червоний
      }
      blinkCount++;
    }

    FastLED.show();
    isOn = !isOn;

    if (!isOn && blinkCount >= 3) {
      leftSide = !leftSide;
      blinkCount = 0;
      lastUpdate = now + pauseBetweenSides;
    }
  }
}
