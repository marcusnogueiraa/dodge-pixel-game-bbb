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
    //          18                                  34
    { {GPIO2, 1, CM_conf_gpmc_clk}, {GPIO2, 17, CM_conf_lcd_data11} },
    //          38                                  37
    { {GPIO2, 15, CM_conf_lcd_data9}, {GPIO2, 14, CM_conf_lcd_data8} },
    //          42                                  41
    { {GPIO2, 11, CM_conf_lcd_data5}, {GPIO2, 10, CM_conf_lcd_data4} },
    //          45                                  46
    { {GPIO2, 6, CM_conf_lcd_data0}, {GPIO2, 7, CM_conf_lcd_data1} }
};

int grid[4][2] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 1},    
};

#endif