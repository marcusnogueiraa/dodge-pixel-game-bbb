#include "game.h"
#include "uart.h"

// Definição das variáveis globais
EntityGpio leftButton = {GPIO1, 13, CM_conf_gpmc_ad13}; // P8-11
EntityGpio rightButton = {GPIO1, 14, CM_conf_gpmc_ad14}; // P8-16
EntityGpio buzzer = {GPIO1, 28, CM_conf_gpmc_ben1}; // P9-12

EntityGpio pixelMapping[4][2] = {
    //          P8-18                             P9-15
    { {GPIO2, 1, CM_conf_gpmc_clk}, {GPIO1, 16, CM_conf_gpmc_a0} },
    //          P8-38                             P9-23
    { {GPIO2, 15, CM_conf_lcd_data9}, {GPIO1, 17, CM_conf_gpmc_a1} },
    //          P8-42                             P9-14
    { {GPIO2, 11, CM_conf_lcd_data5}, {GPIO1, 18, CM_conf_gpmc_a2} },
    //          P8-45                             P9-16
    { {GPIO2, 6, CM_conf_lcd_data0}, {GPIO1, 19, CM_conf_gpmc_a3} }
};

ENTITY grid[GRID_HEIGHT][GRID_WIDTH] = {
    {EMPTY, EMPTY},
    {EMPTY, EMPTY},
    {EMPTY, EMPTY},
    {EMPTY, PLAYER}
};

bool isPlaying = false;
bool blockIsr = false;

void move_left(void) {
    if (blockIsr) {putCh(UART0, 'a'); return;}
    if (!isPlaying) {putCh(UART0, 'b'); return;}
    if (!is_empty(3, 0)) {putCh(UART0, 'c'); return;}
    
    grid[3][RIGHT_COLUMN] = EMPTY;
    grid[3][LEFT_COLUMN] = PLAYER;
}

void move_right(void) {
    if (blockIsr) {putCh(UART0, 'a'); return;}
    if (!isPlaying) {putCh(UART0, 'b'); return;}
    if (!is_empty(3, 1)) {putCh(UART0, 'c'); return;}
    
    grid[3][LEFT_COLUMN] = EMPTY;
    grid[3][RIGHT_COLUMN] = PLAYER;
}

bool is_empty(int i, int j) {
    return grid[i][j] == EMPTY;
}

bool is_obstacle(int i, int j) {
    return grid[i][j] == OBSTACLE;
}
