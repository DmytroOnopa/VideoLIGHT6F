#include "config.h"
#include "display.h"
#include "leds.h"
#include "game.h"
#include "settings.h"
#include "state.h"

int visibleMenuItems = 4;
int menuTopIndex = 0;
int currentMenu = 0;
bool invertDisplay = false;

const char* mainMenu[MENU_COUNT] = {
  "Brightness",
  "Red",
  "Green",
  "Blue",
  "Effect",
  "Rotate Display",
  "Invert Display", 
  "Game",
  "About"          // About тепер останній
};

State state = MENU;
unsigned long lastActivityTime = 0;
int scrollInterval = SCROLL_INTERVAL;
int scrollSpeed = SCROLL_SPEED;
Settings settings;

// Для about screen
int scrollPosY = 10;  // Починаємо знизу
unsigned long lastScrollTime = 0;

void setup() {
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(NEXT_PIN, INPUT_PULLUP);
  
  initLEDs();
  initDisplay();
  
  loadSettings(settings);
  FastLED.setBrightness(settings.brightness);
  
  showLoadingAnimation();
  drawMainMenu();
  lastActivityTime = millis();
}

void loop() {
  if (digitalRead(SELECT_PIN) && digitalRead(NEXT_PIN)) {
    if (state != SCREENSAVER && millis() - lastActivityTime > SCREENSAVER_TIMEOUT) {
      state = SCREENSAVER;
      display.clearDisplay();
      display.display();
    }
  } else {
    lastActivityTime = millis();
    if (state == SCREENSAVER) {
      state = MENU;
      drawMainMenu();
    }
  }

  switch (state) {
    case MENU: handleMenu(); break;
    case ADJUST: handleAdjust(); break;
    case ABOUT: handleAbout(); break;
    case SPACEINVADERS: spaceInvadersUpdate(); break;
    case SPACEINVADERS_GAMEOVER: spaceInvadersGameOverUpdate(); break;
    case SCREENSAVER: screensaverUpdate(); break;
  }

  updateLEDs(settings.effectIndex, settings.currentColor);
  FastLED.show();
  delay(30);
}

void handleMenu() {
  if (!digitalRead(NEXT_PIN)) {
    currentMenu = (currentMenu + 1) % MENU_COUNT;

    if (currentMenu >= menuTopIndex + VISIBLE_MENU_ITEMS) {
      menuTopIndex = currentMenu - VISIBLE_MENU_ITEMS + 1;
    } else if (currentMenu < menuTopIndex) {
      menuTopIndex = currentMenu;
    }

    drawMainMenu();
    delay(200);
  }

if (!digitalRead(SELECT_PIN)) {
    if (currentMenu >= 0 && currentMenu <= 6) {  // Налаштування
      state = ADJUST;
      drawAdjustMenu();
    }
    else if (currentMenu == 7) {  // Game
      state = SPACEINVADERS;
      spaceInvadersInit();
    }
    else if (currentMenu == 8) {  // About
      state = ABOUT;
      drawAbout();
    }
    delay(200);
  }
}

void handleAdjust() {
  static unsigned long lastAdjustTime = 0;
  static bool wasPressed = false;
  static int repeatDelay = 300;

  bool nextPressed = digitalRead(NEXT_PIN) == LOW;
  bool selectPressed = digitalRead(SELECT_PIN) == LOW;

  unsigned long now = millis();

  // 👉 Обробка About
  if (currentMenu == 7) {
    state = ABOUT;
    return;
  }

  if (selectPressed) {
    state = MENU;
    drawMainMenu();
    delay(200);
    return;
  }

  if (nextPressed) {
    if (!wasPressed || (now - lastAdjustTime >= repeatDelay)) {
      switch (currentMenu) {
        case 0:  // Brightness
          settings.brightness += 16;
          if (settings.brightness > 255) settings.brightness = 16;
          FastLED.setBrightness(settings.brightness);
          break;
        case 1:  // Red
          settings.currentColor.r = (settings.currentColor.r + 5) % 256;
          break;
        case 2:  // Green
          settings.currentColor.g = (settings.currentColor.g + 5) % 256;
          break;
        case 3:  // Blue
          settings.currentColor.b = (settings.currentColor.b + 5) % 256;
          break;
        case 4:  // Effect
          settings.effectIndex = (settings.effectIndex + 1) % EFFECT_COUNT;
          break;
        case 5:  // Rotate Display
          settings.rotateDisplay180 = !settings.rotateDisplay180;
          display.setRotation(settings.rotateDisplay180 ? 2 : 0);
          break;
        case 6:  // Invert Display
          settings.invertDisplay = !settings.invertDisplay;
          display.invertDisplay(settings.invertDisplay);
          break;
        case 8:  // About - тут нічого не робимо, бо це просто інформаційний екран
          break;
      }

      saveSettings(settings);
      drawAdjustMenu();

      lastAdjustTime = now;
      wasPressed = true;

      if (repeatDelay > 50) repeatDelay -= 40;
    }
  } else {
    wasPressed = false;
    repeatDelay = 200;
  }
}

void handleAbout() {
  if (!digitalRead(SELECT_PIN)) {
    scrollPosY = SCREEN_HEIGHT;
    state = MENU;
    drawMainMenu();
    delay(200);
  } else {
    drawAbout();
  }
}
