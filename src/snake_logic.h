#pragma once

#include <stdint.h>
#include <stdbool.h>

#define GRID_W 20
#define GRID_H 12
#define MAX_SNAKE 128

typedef struct {
    int x;
    int y;
} Point;

//these are all defined in the .cpp file
extern Point snake[MAX_SNAKE];
extern int snakeLength;

extern Point fruit;

extern int dx;
extern int dy;

extern bool snakeAlive;

//game logic functions
void initGame();
void updateDirection(const char* cmd);
void updateSnake();
bool checkWallCollision();
bool checkSelfCollision();
bool checkFruitCollision();
void spawnFruit();

