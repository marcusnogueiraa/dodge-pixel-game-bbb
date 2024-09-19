#include "interruption.h"
#include "gpio.h"   
#include "uart.h"
#include "timers.h"
#include "game.h"

void ISR_Handler(void) {
    unsigned int irq_number = HWREG(INTC_BASE + INTC_SIR_IRQ) & 0x7F;

    putString(UART0, "[SYS] Tratando Interrupcao\n\r", 28);   
    switch (irq_number){
        case TIMER_ISR_NUMBER: 
            timerIrqHandler(TIMER7);
            break;
        case GPIO_ISR_NUMBER: {
            gpioHandler();
            break;
        }
        default:
            break;
    }
    
    HWREG(INTC_BASE + INTC_CONTROL) = 0x1;
}

int Interrupt_Setup(unsigned int inter) {
    if (inter > 127) return false;

    unsigned int aux = inter / 32; // MIR
    unsigned int offset = inter % 32; // BIT

    unsigned int MIR_CLEAR_REGISTERS[] = {
        INTC_BASE + MIR_CLEAR0,
        INTC_BASE + MIR_CLEAR1,
        INTC_BASE + MIR_CLEAR2,
        INTC_BASE + MIR_CLEAR3
    };

    if (aux < 4) {
        HWREG(MIR_CLEAR_REGISTERS[aux]) |= (1 << offset);
        return true;
    }

    return false;
}