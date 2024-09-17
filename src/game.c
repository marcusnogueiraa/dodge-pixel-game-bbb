#include "game.h"
#include "uart.h"

// Definição das variáveis globais
EntityGpio leftButton = {GPIO1, 13, CM_conf_gpmc_ad13}; // P8-11
EntityGpio rightButton = {GPIO1, 14, CM_conf_gpmc_ad14}; // P8-16
EntityGpio buzzer = {GPIO1, 28, CM_conf_gpmc_ben1}; // P9-12

EntityGpio pixelMapping[4][2] = {
    //          18                                  34
    { {GPIO2, 1, CM_conf_gpmc_clk}, {GPIO2, 17, CM_conf_lcd_data11} },
    //          38                                  37
    { {GPIO2, 15, CM_conf_lcd_data9}, {GPIO2, 14, CM_conf_lcd_data8} },
    //          42                                  41
    { {GPIO2, 11, CM_conf_lcd_data5}, {GPIO2, 10, CM_conf_lcd_data4} },
    //          45                                  46
    { {GPIO2, 6, CM_conf_lcd_data0}, {GPIO2, 7, CM_conf_lcd_data1} }
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
