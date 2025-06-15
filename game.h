#ifndef GAME_H
#define GAME_H

#include "config.h"

struct Bullet {
  int x, y;
  bool active;
};

// Оголосимо всі необхідні змінні як extern
extern int playerX;
extern int playerY;
extern int lives;
extern Bullet bullet;
extern bool invaders[INVADER_ROWS][INVADER_COLS];
extern int invaderX;
extern int invaderY;
extern int invaderDir;
extern int invaderStepDowns;
extern bool gameOver;
extern int gameOverMenuIndex;
extern unsigned long lastShotTime;
extern unsigned long lastMoveTime;

void spaceInvadersInit();
void spaceInvadersUpdate();
void spaceInvadersGameOverUpdate();
void drawGameOver(int gameOverMenuIndex);

#endif