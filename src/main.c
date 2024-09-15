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

#define UART_BUTTON_PIN     
#define SEQUENCE_BUTTON_PIN 

/*-----------------------------------------------------------------------------
*	All Functions
*-----------------------------------------------------------------------------*/

void setupGpio();
unsigned int showUartMenu();
void disableWdt(void);

unsigned int delayInMs = 0;
const int inputUartSize = 5;

/*-----------------------------------------------------------------------------
*  Main Function
*-----------------------------------------------------------------------------*/
int main(void){
	disableWdt();
	setupGpio();

    Interrupt_Setup(95);
    Interrupt_Setup(98);
    Pin_Interrup_Config(leftButton.gpioMod, leftButton.pinNumber, type0);
    Pin_Interrup_Config(rightButton.gpioMod, rightButton.pinNumber, type0);

	timerSetup(TIMER7);

    putString(UART0, "[LOG] Inicializando\n\r", 21);   
    //set_pixel_status(3, 1, HIGH);
   
    while(true){
        putString(UART0, "[LOG] Estado Ocioso\n\r", 21);
        while(!GpioGetPinValue(leftButton.gpioMod, leftButton.pinNumber) 
                && !GpioGetPinValue(rightButton.gpioMod, rightButton.pinNumber));
        log_grid();
        putString(UART0, "[LOG] Jogo Iniciado\n\r", 21);

        isPlaying = true;
        render_frame();
        while(true);
    }

	return 0;
}

void setupGpio(){
	Init_module_gpio(GPIO1);
    Init_module_gpio(GPIO2);

	cmSetCtrlModule(rightButton.controlModule, GPIO_MODULE);
    cmSetCtrlModule(leftButton.controlModule, GPIO_MODULE);

	for(int i = 0; i < 4; i++)
        for(int j = 0; j < 2; j++)
            cmSetCtrlModule(pixelMapping[i][j].controlModule, GPIO_MODULE);

    Init_pin_gpio(rightButton.gpioMod, rightButton.pinNumber, INPUT);
    Init_pin_gpio(leftButton.gpioMod, leftButton.pinNumber, INPUT);

	for(int i = 0; i < 4; i++)
        for(int j = 0; j < 2; j++)
            Init_pin_gpio(pixelMapping[i][j].gpioMod, pixelMapping[i][j].pinNumber, OUTPUT);
}

void set_pixel_status(int i, int j, bool status){
    EntityPixel pixel = pixelMapping[i][j];
    GpioSetPinValue(pixel.gpioMod, pixel.pinNumber, status);
}

void render_frame(){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j <  2; j++){
            bool status = (grid[i][j] != 0);
            EntityPixel *gpio = &pixelMapping[i][j]; 
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

unsigned int showUartMenu(){
    char buffer[inputUartSize]; 

    unsigned int ms = 0;

    putString(UART0, "Defina o tempo de delay (0-9999): ", 35);
    uartGetString(UART0, buffer, inputUartSize);
    putString(UART0, "\n\r", 2);
    buffer[inputUartSize-1] = '\0';  

	int index = 0;
	while(buffer[index] != '\0'){
		if (buffer[index] < '0' || buffer[index] > '9'){
			putString(UART0,"\nERRO\n\r", 7);
			return 1000;
		}
		ms = ms * 10 + (buffer[index] - '0');
		index++;
	}

    return ms;
}

void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}

void ISR_Handler(void) {
    unsigned int irq_number = HWREG(INTC_BASE + INTC_SIR_IRQ) & 0x7F;

    putString(UART0, "[SYS] Tratando Interrupcao\n\r", 28);   
    switch (irq_number){
        case 95: // TIMER
            timerIrqHandler(TIMER7);
            break;
        case 98: { // GPIO1
            unsigned int gpioStatus0 = HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0);
            if (gpioStatus0 & (1 << leftButton.pinNumber)) {
                gpioIsrHandler(GPIO1, type0, leftButton.pinNumber);
                putString(UART0, "[LOG] Left Button\n\r", 19);
                move_left();
                log_grid();
                render_frame();
                return;
            }

            else if (gpioStatus0 & (1 << rightButton.pinNumber)) {
                gpioIsrHandler(GPIO1, type0, rightButton.pinNumber);
                putString(UART0, "[LOG] Right Button\n\r", 20);
                move_right();
                log_grid();
                render_frame();
                return;
            }
            break;
        }
        default:
            break;
    }
    
    HWREG(INTC_BASE + INTC_CONTROL) = 0x1;
}