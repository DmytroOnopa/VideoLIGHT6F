#ifndef CONFIG_H
#define CONFIG_H

// Піни
#define LED_PIN 7
#define LED_COUNT 10
#define SELECT_PIN 6
#define NEXT_PIN 5

#define DISPLAY_TYPE DISPLAY_SSD1306

// Дисплей
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// EEPROM адреси
#define EEPROM_BRIGHTNESS 0
#define EEPROM_RED 1
#define EEPROM_GREEN 2
#define EEPROM_BLUE 3
#define EEPROM_EFFECT 4

// Інші константи
#define EFFECT_COUNT 8
#define MENU_COUNT 9
#define VISIBLE_MENU_ITEMS 4
#define SCREENSAVER_TIMEOUT 30000

// Ігрові константи
#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 4
#define INVADER_ROWS 3
#define INVADER_COLS 6
#define SHOT_INTERVAL 400
#define INVADER_MOVE_INTERVAL 500
#define SCROLL_INTERVAL 100
#define SCROLL_SPEED 1

#endif