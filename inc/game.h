#ifndef GAME_H_
#define GAME_H_

#include "gpio.h"

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

ButtonGpio leftButton = {GPIO1, PIN13, CM_conf_gpmc_ad13};
ButtonGpio rightButton = {GPIO1, PIN14, CM_conf_gpmc_ad14};

EntityPixel pixelMapping[4][2] = {
    { {GPIO2, 6, CM_conf_lcd_data0}, {GPIO2, 7, CM_conf_lcd_data1} },
    { {GPIO2, 9, CM_conf_lcd_data3}, {GPIO2, 10, CM_conf_lcd_data4} },
    { {GPIO2, 11, CM_conf_lcd_data5}, {GPIO2, 12, CM_conf_lcd_data6} },
    { {GPIO2, 13, CM_conf_lcd_data7}, {GPIO2, 14, CM_conf_lcd_data8} }
};

int grid[4][2] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 1},    
};

#endif