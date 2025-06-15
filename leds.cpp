#include "config.h"
#include "display.h"
#include "leds.h"
#include "game.h"
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
    case 4: effectSOS(); break;
    case 5: effectHazard(); break;
    case 6: effectPolice(); break;
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

// === Confetti ===
void effectConfetti() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 30) {
    lastUpdate = millis();
    fadeToBlackBy(leds, LED_COUNT, 10);
    int pos = random8() % LED_COUNT;
    leds[pos] = CHSV(random8(), 255, 255);
  }
}

// === Staboscope ===
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

void effectSOS() {
  static unsigned long t = 0;
  static int s = 0;
  static bool on = false;
  const int seq[] = {200,200,200,200,200,200,600,600,600,600,600,600,200,200,200};
  if (millis() - t < (on ? seq[s] : 200)) return;
  t = millis();
  on = !on;
  if (on) fill_solid(leds, LED_COUNT, CRGB(255,80,0));
  else {
    fill_solid(leds, LED_COUNT, CRGB::Black);
    s = (s + 1) % 15;
  }
  FastLED.show();
}

void effectHazard() {
  static unsigned long lastToggle = 0;
  static bool state = false;

  if (millis() - lastToggle > 500) {
    lastToggle = millis();
    state = !state;
    CRGB color = state ? CRGB(255, 60, 0) : CRGB::Black;

    leds[0] = leds[9] = leds[4] = leds[5] = color;
    // Інші вимикаємо
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

    // Вимикаємо всі
    fill_solid(leds, LED_COUNT, CRGB::Black);

    if (isOn) {
      // моргаємо
      if (leftSide) {
        // Ліва сторона: 0, 1, 9, 8, 7
        leds[0] = leds[1] = leds[9] = leds[8] = leds[7] = CRGB(0, 0, 255); // синій
      } else {
        // Права сторона: 2, 3, 4, 5, 6
        leds[2] = leds[3] = leds[4] = leds[5] = leds[6] = CRGB(255, 0, 0); // червоний
      }
      blinkCount++;
    } else {
      // Пауза між миготінням
    }

    FastLED.show();
    isOn = !isOn;

    if (!isOn && blinkCount >= 3) {
      // 3 блимання завершено — міняємо сторону
      leftSide = !leftSide;
      blinkCount = 0;
      lastUpdate = now + pauseBetweenSides;
    }
  }
}
