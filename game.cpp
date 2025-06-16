#include "game.h"
#include "state.h"
#include "display.h"  // Додано для доступу до дисплея
#include "settings.h" // Додано для доступу до Settings

extern Adafruit_SSD1306 display;
extern State state;
extern Settings settings;  // Тепер правильно

// Глобальні змінні гри
int playerLane = 1;
Car cars[MAX_CARS];
int roadOffset = 0;
unsigned long lastCarSpawn = 0;
bool gameOver = false;

int score = 0;
int gameOverSelection = 0;

void spaceInvadersInit() {
  playerLane = 1;
  gameOver = false;
  
  // Очищення машин
  for (int i = 0; i < MAX_CARS; i++) {
    cars[i].active = false;
  }
  
  lastCarSpawn = millis();
  
  // Ініціалізація генератора випадкових чисел
  randomSeed(analogRead(0));
}

void spaceInvadersUpdate() {
  if (gameOver) return;

  // Обробка керування з захистом від заїдання
  static unsigned long lastInputTime = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastInputTime > 100) {
    if (!digitalRead(NEXT_PIN) && playerLane > 0) {
      playerLane--;
      lastInputTime = currentMillis;
    }
    else if (!digitalRead(SELECT_PIN) && playerLane < ROAD_LANES - 1) {
      playerLane++;
      lastInputTime = currentMillis;
    }
  }

  // Рух дороги
  roadOffset = (roadOffset + 2) % 8;

  // Генерація нових машин
  if (currentMillis - lastCarSpawn > CAR_SPAWN_DELAY) {
    for (int i = 0; i < MAX_CARS; i++) {
      if (!cars[i].active) {
        cars[i].lane = random(0, ROAD_LANES);
        int laneWidth = display.width() / ROAD_LANES;
        cars[i].x = (laneWidth * cars[i].lane) + (laneWidth / 2) - (CAR_WIDTH / 2);
        cars[i].y = -CAR_HEIGHT;
        cars[i].active = true;
        lastCarSpawn = currentMillis;
        break;
      }
    }
  }

  // Оновлення позицій машин
  for (int i = 0; i < MAX_CARS; i++) {
    if (cars[i].active) {
      cars[i].y += 3;  // Швидкість машин

      // Перевірка на зіткнення
      if (cars[i].y + CAR_HEIGHT > display.height() - PLAYER_HEIGHT &&
          cars[i].y < display.height() - PLAYER_HEIGHT + 5 &&
          cars[i].lane == playerLane) {
        gameOver = true;
        state = SPACEINVADERS_GAMEOVER;  // ← ОЦЕ ГОЛОВНЕ
        return;
      }

      // Видалення машин, що виїхали за межі
      if (cars[i].y > display.height()) {
        cars[i].active = false;
        score++; // Додаємо бал за уникнуту машину
      }

    }
  }
}

void spaceInvadersDraw() {
  display.clearDisplay();

  int laneWidth = display.width() / ROAD_LANES;

  // Малювання дороги з розміткою
  for (int i = 1; i < ROAD_LANES; i++) {
    int x = i * laneWidth;
    for (int y = roadOffset; y < display.height(); y += 12) {
      display.drawFastVLine(x, y, 6, SSD1306_WHITE);
    }
  }

  // Малювання машин
  for (int i = 0; i < MAX_CARS; i++) {
    if (cars[i].active) {
      display.fillRoundRect(cars[i].x, cars[i].y, CAR_WIDTH, CAR_HEIGHT, 2, SSD1306_WHITE);
    }
  }

  // Малювання гравця з фарами
  int playerX = (laneWidth * playerLane) + (laneWidth / 2) - (PLAYER_WIDTH / 2);
  int playerY = display.height() - PLAYER_HEIGHT;
  display.fillRect(playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, SSD1306_WHITE);
  display.fillRect(playerX + 2, playerY - 2, 3, 2, SSD1306_WHITE);  // Ліва фара
  display.fillRect(playerX + PLAYER_WIDTH - 5, playerY - 2, 3, 2, SSD1306_WHITE);  // Права фара

  // Малювання HUD
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("SCORE: ");
  display.print(score);

  // Game Over меню
  if (gameOver) {
    const char* options[2] = { "New Game", "Exit" };
    for (int i = 0; i < 2; i++) {
      int boxX = display.width() / 2 - 30;
      int boxY = display.height() / 2 + i * 12;
      if (i == gameOverSelection) {
        display.fillRect(boxX, boxY, 60, 10, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      display.setCursor(boxX + 5, boxY + 1);
      display.print(options[i]);
    }
  }

  display.display();
}

bool spaceInvadersIsGameOver() {
  return gameOver;
}

void spaceInvadersGameOverUpdate() {
  static unsigned long lastInputTime = 0;
  static int selectedOption = 0; // 0 = New Game, 1 = Exit
  unsigned long now = millis();

  // Обробка вводу тільки якщо минуло достатньо часу
  if ((now - lastInputTime) > 200) {
    if (!digitalRead(NEXT_PIN)) {
      selectedOption = 1 - selectedOption; // toggle
      lastInputTime = now;
    }

    if (!digitalRead(SELECT_PIN)) {
      if (selectedOption == 0) {
        spaceInvadersInit();
        state = SPACEINVADERS;
      } else {
        state = MENU;
        drawMainMenu();
      }
      lastInputTime = now;
      return; // Вийти, щоб уникнути зайвого оновлення дисплея
    }
  }

  // Оновлення дисплея
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(display.width() / 2 - 48, 8);
  display.print("GAME OVER");

  display.setTextSize(1);
  display.setCursor(display.width() / 2 - 30, 32);
  display.print(selectedOption == 0 ? "> New Game" : "  New Game");

  display.setCursor(display.width() / 2 - 30, 44);
  display.print(selectedOption == 1 ? "> Exit" : "  Exit");

  display.display();
}
