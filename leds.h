#ifndef LEDS_H
#define LEDS_H

#include <FastLED.h>
#include "config.h"

extern CRGB leds[LED_COUNT];

void initLEDs();
void updateLEDs(int effectIndex, CRGB currentColor);
const char* effectName(int idx);
void effectStatic(CRGB color);
void effectRunningDot(CRGB color);
void effectConfetti();
void effectStaboscope();
void effectSoftGlow();
void effectCyberpunk();
void effectHazard();
void effectPolice();

#endif