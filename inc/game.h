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

typedef struct EntityGpio {
    gpioMod gpioMod;
    unsigned char pinNumber;
    CONTROL_MODULE controlModule;
} EntityGpio;

extern EntityGpio leftButton;
extern EntityGpio rightButton;
extern EntityGpio buzzer;
extern EntityGpio pixelMapping[4][2];

typedef enum STATE {
    GENERATE_STATE = 0,
    DOWN_OBSTACLES_1 = 1,
    DOWN_OBSTACLES_2 = 2,
    DOWN_OBSTACLES_3 = 3
} STATE;

extern ENTITY grid[GRID_HEIGHT][GRID_WIDTH];

extern bool isPlaying;
extern bool blockIsr;

void move_left(void);
void move_right(void);
bool is_empty(int i, int j);
bool is_obstacle(int i, int j);

#endif // GAME_H_