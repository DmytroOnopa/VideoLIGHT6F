#ifndef STATE_H
#define STATE_H

#include <FastLED.h>  // бо тип CRGB

enum State {
  MENU,
  ADJUST,
  ABOUT,
  SPACEINVADERS,
  SPACEINVADERS_GAMEOVER,
  SCREENSAVER
};

extern CRGB currentColor; // <--- це робить currentColor доступною скрізь
// ОГОЛОШЕННЯ ЗМІННИХ, які реально є в VideoLIGHT6.ino
extern State state;

extern int currentMenu;
extern int menuTopIndex;
extern int visibleMenuItems;

extern int brightness;
extern int effectIndex;
extern bool invertDisplay;

extern int scrollPosY;
extern unsigned long lastScrollTime;
extern int scrollInterval;
extern int scrollSpeed;

extern unsigned long lastActivityTime;

#endif