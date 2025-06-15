#include "config.h"
#include "display.h"
#include "leds.h"
#include "game.h"
#include "settings.h"
#include "state.h"
#include <Arduino.h>

// Оголошення зовнішніх змінних
extern State state;
extern Settings settings;
extern int currentMenu;
extern int menuTopIndex;

// Ігрові змінні
int playerX = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
int playerY = SCREEN_HEIGHT - PLAYER_HEIGHT - 2;
int lives = 3;
Bullet bullet;
bool invaders[INVADER_ROWS][INVADER_COLS];
int invaderX = 0;
int invaderY = 10;
int invaderDir = 1;
int invaderStepDowns = 0;
bool gameOver = false;
int gameOverMenuIndex = 0;
unsigned long lastShotTime = 0;
unsigned long lastMoveTime = 0;

void spaceInvadersInit() {
    playerX = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    playerY = SCREEN_HEIGHT - PLAYER_HEIGHT - 2;
    lives = 3;
    bullet.active = false;
    invaderX = 0;
    invaderY = 10;
    invaderDir = 1;
    invaderStepDowns = 0;
    gameOver = false;
    lastShotTime = 0;
    lastMoveTime = 0;

    for (int row = 0; row < INVADER_ROWS; row++) {
        for (int col = 0; col < INVADER_COLS; col++) {
            invaders[row][col] = true;
        }
    }
}

void spaceInvadersUpdate() {
  // Керування гравцем
  if (!digitalRead(SELECT_PIN)) {
    playerX = constrain(playerX - 2, 0, SCREEN_WIDTH - PLAYER_WIDTH);
  }
  if (!digitalRead(NEXT_PIN)) {
    playerX = constrain(playerX + 2, 0, SCREEN_WIDTH - PLAYER_WIDTH);
  }

  // Автоматична стрільба
  if (!bullet.active && millis() - lastShotTime > SHOT_INTERVAL) {
    bullet.x = playerX + PLAYER_WIDTH / 2;
    bullet.y = playerY - 1;
    bullet.active = true;
    lastShotTime = millis();
  }

  // Рух кулі
  if (bullet.active) {
    bullet.y -= 3;
    if (bullet.y < 0) bullet.active = false;
  }

  // Рух ворогів
  if (millis() - lastMoveTime > INVADER_MOVE_INTERVAL) {
    lastMoveTime = millis();
    invaderX += invaderDir * 5;

    // Перевірка досягнення краю екрану
    if (invaderX + INVADER_COLS * 12 > SCREEN_WIDTH || invaderX < 0) {
      invaderDir *= -1;
      invaderY += 6;
      invaderStepDowns++;

      if (invaderStepDowns >= 3) {
        lives = 0;
        gameOver = true;
      }
    }
  }

  // Перевірка попадання кулі у ворога
  if (bullet.active) {
    for (int row = 0; row < INVADER_ROWS; row++) {
      for (int col = 0; col < INVADER_COLS; col++) {
        if (invaders[row][col]) {
          int ix = invaderX + col * 12;
          int iy = invaderY + row * 8;

          if (bullet.x >= ix && bullet.x <= ix + 8 &&
              bullet.y >= iy && bullet.y <= iy + 5) {
            invaders[row][col] = false;
            bullet.active = false;
          }
        }
      }
    }
  }

  // Перевірка зіткнення гравця з ворогом
  for (int row = 0; row < INVADER_ROWS; row++) {
    for (int col = 0; col < INVADER_COLS; col++) {
      if (invaders[row][col]) {
        int ix = invaderX + col * 12;
        int iy = invaderY + row * 8;
        if (iy + 5 >= playerY &&
            ix + 8 >= playerX &&
            ix <= playerX + PLAYER_WIDTH) {
          lives--;
          invaderStepDowns++;
          if (lives <= 0) gameOver = true;
          break;
        }
      }
    }
  }

  // Перевірка завершення гри
  if (gameOver) {
      state = SPACEINVADERS_GAMEOVER;
      drawGameOver(gameOverMenuIndex);
      return;
  }

  // Малювання гри
  display.clearDisplay();

  // Гравець
  display.fillRect(playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, SSD1306_WHITE);

  // Куля
  if (bullet.active) {
    display.drawPixel(bullet.x, bullet.y, SSD1306_WHITE);
  }

  // Вороги
  for (int row = 0; row < INVADER_ROWS; row++) {
    for (int col = 0; col < INVADER_COLS; col++) {
      if (invaders[row][col]) {
        int ix = invaderX + col * 12;
        int iy = invaderY + row * 8;
        display.drawRect(ix, iy, 8, 5, SSD1306_WHITE);
      }
    }
  }

  // Життя
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Lives: ");
  display.print(lives);

  display.display();
}

void drawGameOver(int gameOverMenuIndex) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 20);
    display.println("Game Over");

    display.setTextSize(1);
    const char* options[] = { "New Game", "Exit" };
    int yPositions[] = { 45, 55 };

    for (int i = 0; i < 2; i++) {
        if (i == gameOverMenuIndex) {
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(20, yPositions[i]);
        display.println(options[i]);
    }

    display.display();
}

void spaceInvadersGameOverUpdate() {
    if (!digitalRead(NEXT_PIN)) {
        gameOverMenuIndex = (gameOverMenuIndex + 1) % 2;
        drawGameOver(gameOverMenuIndex);
        delay(200);
    }
    
    if (!digitalRead(SELECT_PIN)) {
        if (gameOverMenuIndex == 0) {
            spaceInvadersInit();
            state = SPACEINVADERS;
        } else {
            state = MENU;
            drawMainMenu();
        }
        delay(200);
    }
}
