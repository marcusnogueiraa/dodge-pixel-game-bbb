#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "control_module.h"
#include "timers.h"
#include "uart.h"
#include "pad.h"
#include "game.h"
#include "interruption.h"

#define GPIO_MODULE     7

/*-----------------------------------------------------------------------------
*	All Functions
*-----------------------------------------------------------------------------*/

void log_grid();
void setupGpio();
void disableWdt(void);
void idle_mode();
void render_frame();
void reset_grid();
void update_delay(unsigned int *delay);

bool firstTimePlaying = false;

/*-----------------------------------------------------------------------------
*  Main Function
*-----------------------------------------------------------------------------*/
int main(void){
	disableWdt();
	timerSetup(TIMER7);
    timerEnable(TIMER7);
    Interrupt_Setup(TIMER_ISR_NUMBER);
	
    setupGpio();

    putString(UART0, "[LOG] Inicializando\n\r", 21);   
    blockIsr = false;

    while(true){
        unsigned int delayInMs = 300;
        idle_mode();
        isPlaying = true;
        
        STATE current_state = STATE_0;
        putString(UART0, "[LOG] Jogo Iniciado\n\r", 21);
        while(isPlaying){
            match_tasks(&current_state);
            delay(delayInMs, TIMER7);
            update_delay(&delayInMs);
        }
    }

	return 0;
}

void setupGpio(){
	Init_module_gpio(GPIO1);
    Init_module_gpio(GPIO2);

	cmSetCtrlModule(rightButton.controlModule, GPIO_MODULE);
    cmSetCtrlModule(leftButton.controlModule, GPIO_MODULE);
    cmSetCtrlModule(buzzer.controlModule, GPIO_MODULE);

	for(int i = 0; i < 4; i++)
        for(int j = 0; j < 2; j++)
            cmSetCtrlModule(pixelMapping[i][j].controlModule, GPIO_MODULE);

    Init_pin_gpio(rightButton.gpioMod, rightButton.pinNumber, INPUT);
    Init_pin_gpio(leftButton.gpioMod, leftButton.pinNumber, INPUT);

    Init_pin_gpio(buzzer.gpioMod, buzzer.pinNumber, OUTPUT);

	for(int i = 0; i < 4; i++)
        for(int j = 0; j < 2; j++)
            Init_pin_gpio(pixelMapping[i][j].gpioMod, pixelMapping[i][j].pinNumber, OUTPUT);

    Interrupt_Setup(GPIO_ISR_NUMBER);
    Pin_Interrup_Config(leftButton.gpioMod, leftButton.pinNumber, type0);
    Pin_Interrup_Config(rightButton.gpioMod, rightButton.pinNumber, type0);
}

void update_delay(unsigned int *delay){
    putString(UART0, "[SYS] delay: ", 13);
    
    putInt(UART0, *delay);
    if (*delay > 100) *delay -= 1;

    putCh(UART0, '\r');
    putCh(UART0, '\n');
}

void idle_mode(){
    putString(UART0, "[LOG] Estado Ocioso\n\r", 21);

    reset_grid();
    render_frame();
    log_grid();

    blockIsr = true;
    while(!GpioGetPinValue(leftButton.gpioMod, leftButton.pinNumber) 
            && !GpioGetPinValue(rightButton.gpioMod, rightButton.pinNumber));
    
    if (GpioGetPinValue(leftButton.gpioMod, leftButton.pinNumber)) {
        grid[3][LEFT_COLUMN] = PLAYER;
        grid[3][RIGHT_COLUMN] = EMPTY;
    }
    blockIsr = false;
    
    if (firstTimePlaying){
        seed = timerRead(TIMER7);
        if (seed == 0) seed = 123;

        putString(UART0, "Seed: \n\r", 8);
        putInt(UART0, seed);
        putCh(UART0, '\n');
        putCh(UART0, '\r');
        timerDisable(TIMER7);
        firstTimePlaying = false;
    } 
}

void reset_grid(){
    for(int i = 0; i < GRID_HEIGHT; i++){
        for(int j = 0; j < GRID_WIDTH; j++){
            grid[i][j] = EMPTY;
        }
    }
    grid[3][RIGHT_COLUMN] = PLAYER;
}

void match_tasks(STATE *state){
    handle_state(state);
    render_frame();
    log_grid();
}

void set_pixel_status(int i, int j, bool status){
    EntityGpio pixel = pixelMapping[i][j];
    GpioSetPinValue(pixel.gpioMod, pixel.pinNumber, status);
}

void render_frame(){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j <  2; j++){
            bool status = (grid[i][j] != 0);
            EntityGpio *gpio = &pixelMapping[i][j]; 
            GpioSetPinValue(gpio->gpioMod, gpio->pinNumber, status);
        }
    }
}

void log_grid(){
    putString(UART0, "\n\r", 2);
    putString(UART0, "[LOG] GameGrid:\n\r", 17);
    for(int i = 0; i < GRID_HEIGHT; i++){
        for(int j = 0; j < GRID_WIDTH; j++){
            unsigned char id = '2';
            if (grid[i][j] == OBSTACLE) id = '1';
            else if (grid[i][j] == EMPTY) id = '0';

            putCh(UART0, id);
        }
        putString(UART0, "\n\r", 2);
    }
    putString(UART0, "\n\r", 2);
}