#ifndef GAME_H_
#define GAME_H_

#include "gpio.h"

#define GRID_HEIGHT 4
#define GRID_WIDTH 2

#define LEFT_COLUMN 0
#define RIGHT_COLUMN 1

#define PLAYER_LINE (GRID_HEIGHT-1)

typedef enum ENTITY {
    EMPTY = 0,
    OBSTACLE = 1,
    PLAYER = 2
} ENTITY;

typedef struct EntityPixel {
    gpioMod gpioMod;
    unsigned char pinNumber;
    CONTROL_MODULE controlModule;
} EntityPixel;

typedef struct ButtonGpio {
    gpioMod gpioMod;
    unsigned char pinNumber;
    CONTROL_MODULE controlModule;
} ButtonGpio;

extern ButtonGpio leftButton;
extern ButtonGpio rightButton;
extern EntityPixel pixelMapping[4][2];

typedef enum STATE {
    GENERATE_STATE = 0,
    DOWN_OBSTACLES_1 = 1,
    DOWN_OBSTACLES_2 = 2,
    DOWN_OBSTACLES_3 = 3
} STATE;

extern ENTITY grid[GRID_HEIGHT][GRID_WIDTH];

extern bool isPlaying;

void move_left(void);
void move_right(void);
bool is_empty(int i, int j);
bool is_obstacle(int i, int j);

#endif // GAME_H_