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

typedef enum STATE {
    STATE_0 = 0,
    STATE_1 = 1,
    STATE_2 = 2,
    STATE_3 = 3
} STATE;

extern EntityGpio leftButton;
extern EntityGpio rightButton;
extern EntityGpio buzzer;
extern EntityGpio pixelMapping[4][2];

extern unsigned int seed;
extern ENTITY grid[GRID_HEIGHT][GRID_WIDTH];
extern bool isPlaying;
extern bool blockIsr;

void knock_song();
void move_left(void);
void move_right(void);
bool is_empty(int i, int j);
bool is_obstacle(int i, int j);
bool verify_colision(unsigned char column);
void knock_animation(int column);
void update_state(STATE *state);
void handle_state(STATE *state);
unsigned int random_number();
unsigned char get_random_column();

#endif // GAME_H_