#include "snake_logic.h"
#include <string.h>
#include <stdlib.h>

Point snake[MAX_SNAKE];
int snakeLength = 3;

Point fruit;

int dx = 1; //starts by going right
int dy = 0;

bool snakeAlive = false;

void initGame() {
    snakeLength = 3;
    snake[0] = {5, 6};
    snake[1] = {4, 6};
    snake[2] = {3, 6};

    dx = 1;
    dy = 0;
    snakeAlive = true;

    spawnFruit();
}

void spawnFruit() {
    fruit.x = rand() % GRID_W;
    fruit.y = rand() % GRID_H;
}

void updateDirection(const char* cmd) {
    if (strcmp(cmd, "up") == 0 && dy != 1) {
        dx = 0; dy = -1;
    }
    else if (strcmp(cmd, "down") == 0 && dy != -1) {
        dx = 0; dy = 1;
    }
    else if (strcmp(cmd, "left") == 0 && dx != 1) {
        dx = -1; dy = 0;
    }
    else if (strcmp(cmd, "right") == 0 && dx != -1) {
        dx = 1; dy = 0;
    }
}

bool checkWallCollision() {
    return (
        snake[0].x < 0 || snake[0].x >= GRID_W-1 ||
        snake[0].y < 0 || snake[0].y >= GRID_H-1
    );
}

bool checkSelfCollision() {
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return true;
        }
    }
    return false;
}

bool checkFruitCollision() {
    return (snake[0].x == fruit.x && snake[0].y == fruit.y);
}

void updateSnake() {
    if (!snakeAlive) return;
    //shift the body positions
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    //move the head based on dx/dy
    snake[0].x += dx;
    snake[0].y += dy;

    if (checkWallCollision() || checkSelfCollision()) {
        snakeAlive = false;
        return;
    }
    if (checkFruitCollision()) {
        snakeLength++;
        spawnFruit();
    }
}