#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <FastLED.h>
#include "config.h"

extern Adafruit_SSD1306 display;
extern const char* mainMenu[MENU_COUNT];
extern const char* const aboutText[] PROGMEM;

void initDisplay();
void showLoadingAnimation();
void drawMainMenu();
void drawAdjustMenu();
void drawAbout();
void screensaverUpdate();
const char* effectName(int idx);

#endif