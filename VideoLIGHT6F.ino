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
    case MENU:
      handleMenu();
      break;

    case ADJUST:
      handleAdjust();
      break;

    case ABOUT:
      handleAbout();
      break;

    case SPACEINVADERS:
      spaceInvadersUpdate();
      spaceInvadersDraw(); // ← ДОДАНО! Малює гру
      break;

    case SPACEINVADERS_GAMEOVER:
      spaceInvadersGameOverUpdate();
      // spaceInvadersDraw(); // ← ДОДАНО! Показує GAME OVER
      break;

    case SCREENSAVER:
      screensaverUpdate();
      break;
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
  static int repeatDelay = 400;  // Початкова затримка більша для комфорту
  static int step = 1;          // Динамічний крок зміни

  bool nextPressed = digitalRead(NEXT_PIN) == LOW;
  bool selectPressed = digitalRead(SELECT_PIN) == LOW;
  unsigned long now = millis();

  // Вихід з режиму налаштувань
  if (selectPressed) {
    state = MENU;
    drawMainMenu();
    delay(200);
    return;
  }

  // Обробка натискання NEXT
  if (nextPressed) {
    if (!wasPressed || (now - lastAdjustTime >= repeatDelay)) {
      // Збільшення швидкості зміни при утриманні кнопки
      if (wasPressed && repeatDelay > 100) {
        repeatDelay -= 20;  // Плавне прискорення
        step = map(repeatDelay, 400, 100, 1, 10);  // Крок зростає від 1 до 10
      }

      switch (currentMenu) {
        case 0:  // Brightness
          settings.brightness = (settings.brightness + step) % 256;
          FastLED.setBrightness(settings.brightness);
          EEPROM.update(EEPROM_BRIGHTNESS, settings.brightness);
          break;
          
        case 1:  // Red
          settings.currentColor.r = (settings.currentColor.r + step) % 256;
          EEPROM.update(EEPROM_RED, settings.currentColor.r);
          break;
          
        case 2:  // Green
          settings.currentColor.g = (settings.currentColor.g + step) % 256;
          EEPROM.update(EEPROM_GREEN, settings.currentColor.g);
          break;
          
        case 3:  // Blue
          settings.currentColor.b = (settings.currentColor.b + step) % 256;
          EEPROM.update(EEPROM_BLUE, settings.currentColor.b);
          break;
          
        case 4:  // Effect
          settings.effectIndex = (settings.effectIndex + 1) % EFFECT_COUNT;
          EEPROM.update(EEPROM_EFFECT, settings.effectIndex);
          step = 1;  // Скидаємо крок для ефектів
          break;
          
        case 5:  // Rotate Display
          settings.rotateDisplay = !settings.rotateDisplay;
          display.setRotation(settings.rotateDisplay ? 2 : 0);
          break;
  
        case 6:  // Invert Display
          settings.invertDisplay = !settings.invertDisplay;
          display.invertDisplay(settings.invertDisplay);
          break;
      }

      drawAdjustMenu();
      lastAdjustTime = now;
      wasPressed = true;
    }
  } else {
    // Скидання швидкості при відпусканні кнопки
    wasPressed = false;
    repeatDelay = 400;
    step = 1;
  }
}

void handleAbout() {
  if (!digitalRead(SELECT_PIN)) {
    state = MENU;
    drawMainMenu();
  } else {
    drawAbout(); // Цей виклик має відбуватися постійно
  }
}
