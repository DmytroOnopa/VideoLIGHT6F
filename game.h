#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "state.h"

// Константи гри
#define ROAD_LANES 3
#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 8
#define CAR_WIDTH 12
#define CAR_HEIGHT 8
#define MAX_CARS 5
#define CAR_SPAWN_DELAY 1000

extern int score;
extern int gameOverSelection;

struct Car {
  int x, y;
  int lane;
  bool active;
};

void spaceInvadersInit();
void spaceInvadersUpdate();
void spaceInvadersDraw();
bool spaceInvadersIsGameOver();
void spaceInvadersGameOverUpdate();

#endif