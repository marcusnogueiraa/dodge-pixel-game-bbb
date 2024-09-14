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

	timerSetup(TIMER7);

    putString(UART0, "- Inicializando\n", 16);    
    while(true){
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 2; j++){
                EntityPixel pixel = pixelMapping[i][j];
                GpioSetPinValue(pixel.gpioMod, pixel.pinNumber, HIGH);
                delay(500, TIMER7);
                GpioSetPinValue(pixel.gpioMod, pixel.pinNumber, LOW);
            }
        }
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

void render_frame(){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j <  2; j++){
            bool status = (grid[i][j] != 0);
            EntityPixel *gpio = &pixelMapping[i][j]; 
            GpioSetPinValue(gpio->gpioMod, gpio->pinNumber, status);
        }
    }
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
    
    switch (irq_number){
        case 95: // TIMER
            timerIrqHandler(TIMER7);
            break;
        case 98: { // GPIO1
            unsigned int gpioStatus0 = HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0);
            if (gpioStatus0 & (1 << 1)) {
                gpioIsrHandler(GPIO1, type0, 0);
                return;
            }
            break;
        }
        default:
            break;
    }
    
    HWREG(INTC_BASE + INTC_CONTROL) = 0x1;
}