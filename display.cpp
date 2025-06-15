#include "config.h"
#include "display.h"
#include "leds.h"
#include "game.h"
#include "settings.h"
#include "state.h"
#include <Arduino.h>

extern int visibleMenuItems;
extern int menuTopIndex;
extern int currentMenu;
extern const char* mainMenu[];  // масив рядків
extern bool invertDisplay;

const String aboutText = 
  "Git: DmytroOnopa\n"
  "FW: VideoLIGHT6F";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while(1);
  }
  display.invertDisplay(EEPROM.read(EEPROM_INVERT));
}

int countLines(const String& text) {
  int count = 1;
  for (unsigned int i = 0; i < text.length(); i++) {
    if (text[i] == '\n') count++;
  }
  return count;
}

void showLoadingAnimation() {
  const char* text = "theGrove";
  int len = strlen(text);
  for (int frame = 0; frame < 40; frame++) {
    display.clearDisplay();
    // Рухомі смужки
    for (int y = 0; y < SCREEN_HEIGHT; y += 6) {
      display.drawFastHLine(0, (y + frame * 3) % SCREEN_HEIGHT, SCREEN_WIDTH, SSD1306_WHITE);
    }
    // Текст з "гліч" ефектом
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    for (int i = 0; i < len; i++) {
      int x = (SCREEN_WIDTH - len * 12) / 2 + i * 12;
      int y = SCREEN_HEIGHT / 2 - 8;
      if (random(10) > 7 || frame > 30) {
        display.setCursor(x, y);
        display.print(text[i]);
      } else {
        display.setCursor(x, y + random(-2,3));
        display.print((char)random(33, 126));
      }
    }
    display.display();
    delay(40);
  }
  delay(600);
}

void drawMainMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  
  // Заголовок
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("VideoLIGHT Settings:"));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  // Відображаємо видимі пункти меню
  for (int i = 0; i < visibleMenuItems; i++) {
    int itemIndex = menuTopIndex + i;
    if (itemIndex >= MENU_COUNT) break;
    
    display.setCursor(0, 12 + i*10);
    if (itemIndex == currentMenu) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    
    display.print(mainMenu[itemIndex]);
    
  }

  // Індикатор прокрутки (якщо меню довше ніж видима область)
  if (MENU_COUNT > visibleMenuItems) {
    int scrollbarHeight = visibleMenuItems * 10 - 2;
    int scrollbarPos = map(currentMenu, 0, MENU_COUNT-1, 12, 12 + scrollbarHeight - 4);
    display.drawLine(SCREEN_WIDTH-4, 12, SCREEN_WIDTH-4, 12 + scrollbarHeight, SSD1306_WHITE);
    display.fillRect(SCREEN_WIDTH-3, scrollbarPos, 3, 3, SSD1306_WHITE);
  }
  
    display.setTextColor(SSD1306_WHITE); // <-- оце обов'язково
    display.drawLine(0, 52, SCREEN_WIDTH, 52, SSD1306_WHITE);
    display.setCursor(19, 54);
    display.print(F("SELECT | CHANGE"));

  display.display();
}

void drawAdjustMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 2);
  display.print(F("Edit: "));
  display.println(mainMenu[currentMenu]);
  display.drawLine(0, 12, SCREEN_WIDTH, 12, SSD1306_WHITE);

  display.setCursor(0, 20);
  
  switch (currentMenu) {
    case 0: // Brightness
      display.print(F("Brightness: "));
      display.println(settings.brightness);
      break;
    case 1: // Red
      display.print(F("Red: "));
      display.println(settings.currentColor.r);
      break;
    case 2: // Green
      display.print(F("Green: "));
      display.println(settings.currentColor.g);
      break;
    case 3: // Blue
      display.print(F("Blue: "));
      display.println(settings.currentColor.b);
      break;
    case 4: // Effect
      display.print(F("Effect: "));
      display.println(effectName(settings.effectIndex));
      break;
    case 5: // Rotate
      display.print(F("Rotate display: "));
      display.println(invertDisplay ? "0" : "180");
      break;
    case 6: // Invert
      display.print(F("Enable light mode: "));
      display.println(invertDisplay ? "Yes" : "No");
      break;
    case 7: // About
      display.print(F("About screen"));
      break;
  }

  // Нижній текст з інструкцією
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(0, 52, SCREEN_WIDTH, 52, SSD1306_WHITE);
  display.setCursor(0, 54);
  display.print(F("Sel:Back Next:Change"));
  
  display.display();
}

const char* effectName(int idx) {
  switch (idx) {
    case 0: return "Static";
    case 1: return "Running Dot";
    case 2: return "Confetti";
    case 3: return "Staboscope";
    case 4: return "SOS Signal";
    case 5: return "Hazard Light";
    case 6: return "Police Mode";
    default: return "Unknown";
  }
}

void drawAbout() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  const int lineHeight = 10;
  int y = 2;

  int from = 0;
  while (y < SCREEN_HEIGHT - 10) {
    int to = aboutText.indexOf('\n', from);
    if (to == -1) break;

    display.setCursor(0, y);
    display.print(aboutText.substring(from, to));
    
    y += lineHeight;
    from = to + 1;
  }

  // Підказка внизу
  display.drawLine(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, SCREEN_HEIGHT - 10, SSD1306_WHITE);
  display.setCursor(0, SCREEN_HEIGHT - 8);
  display.print(F("Press SELECT to back"));

  display.display();
}

void screensaverUpdate() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Центрований заголовок
  display.setCursor((SCREEN_WIDTH - 84) / 2, 2); // 84 — ширина тексту приблизно
  display.print(F("geniusbar.site/"));

  // Лінія + рамка
  display.drawLine(0, 15, SCREEN_WIDTH, 15, SSD1306_WHITE);
  display.drawRect(0, 20, SCREEN_WIDTH, 38, SSD1306_WHITE);

  // RGB
  display.setCursor(5, 24);
  display.print(F("RGB: "));
  display.print(settings.currentColor.r);
  display.print(',');
  display.print(settings.currentColor.g);
  display.print(',');
  display.print(settings.currentColor.b);

  // Яскравість
  display.setCursor(5, 36);
  display.print(F("Brightness: "));
  display.print(settings.brightness);
  display.print(F("/255"));

  // Прогрес-бар
  uint8_t bar = map(settings.brightness, 0, 255, 0, SCREEN_WIDTH - 10);
  display.drawRect(5, 48, SCREEN_WIDTH - 10, 5, SSD1306_WHITE);
  display.fillRect(5, 48, bar, 5, SSD1306_WHITE);

  display.display();
}
