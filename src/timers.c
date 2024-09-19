#include "timers.h"
#include "interruption.h"
#include "gpio.h"

bool flag_timer;

void DMTimerWaitForWrite(unsigned int count, Timer timer) {
    unsigned int timerBaseAddr;
    switch (timer) {
        case TIMER2: timerBaseAddr = SOC_DMTIMER_2_REGS; break;
        case TIMER3: timerBaseAddr = SOC_DMTIMER_3_REGS; break;
        case TIMER4: timerBaseAddr = SOC_DMTIMER_4_REGS; break;
        case TIMER5: timerBaseAddr = SOC_DMTIMER_5_REGS; break;
        case TIMER6: timerBaseAddr = SOC_DMTIMER_6_REGS; break;
        case TIMER7: timerBaseAddr = SOC_DMTIMER_7_REGS; break;
        default: return;
    }
    while (count & HWREG(timerBaseAddr + DMTIMER_TWPS));
}

void timerEnable(Timer timer) {
    unsigned int timerBaseAddr;
    switch (timer) {
        case TIMER2: timerBaseAddr = SOC_DMTIMER_2_REGS; break;
        case TIMER3: timerBaseAddr = SOC_DMTIMER_3_REGS; break;
        case TIMER4: timerBaseAddr = SOC_DMTIMER_4_REGS; break;
        case TIMER5: timerBaseAddr = SOC_DMTIMER_5_REGS; break;
        case TIMER6: timerBaseAddr = SOC_DMTIMER_6_REGS; break;
        case TIMER7: timerBaseAddr = SOC_DMTIMER_7_REGS; break;
        default: return;
    }
    DMTimerWaitForWrite(0x1, timer);
    HWREG(timerBaseAddr + DMTIMER_TCLR) |= 0x1;
}

void timerDisable(Timer timer) {
    unsigned int timerBaseAddr;
    switch (timer) {
        case TIMER2: timerBaseAddr = SOC_DMTIMER_2_REGS; break;
        case TIMER3: timerBaseAddr = SOC_DMTIMER_3_REGS; break;
        case TIMER4: timerBaseAddr = SOC_DMTIMER_4_REGS; break;
        case TIMER5: timerBaseAddr = SOC_DMTIMER_5_REGS; break;
        case TIMER6: timerBaseAddr = SOC_DMTIMER_6_REGS; break;
        case TIMER7: timerBaseAddr = SOC_DMTIMER_7_REGS; break;
        default: return;
    }
    DMTimerWaitForWrite(0x1, timer);
    HWREG(timerBaseAddr + DMTIMER_TCLR) &= ~0x1;
}

void timerSetup(Timer timer) {
    unsigned int clkCtrlReg;
    switch (timer) {
        case TIMER2: clkCtrlReg = TIMER2_CLKCTRL; break;
        case TIMER3: clkCtrlReg = TIMER3_CLKCTRL; break;
        case TIMER4: clkCtrlReg = TIMER4_CLKCTRL; break;
        case TIMER5: clkCtrlReg = TIMER5_CLKCTRL; break;
        case TIMER6: clkCtrlReg = TIMER6_CLKCTRL; break;
        case TIMER7: clkCtrlReg = TIMER7_CLKCTRL; break;
        default: return;
    }
    HWREG(SOC_CM_PER_REGS + clkCtrlReg) |= 0x2;
    while ((HWREG(SOC_CM_PER_REGS + clkCtrlReg) & 0x3) != 0x2);
}

void timerIrqHandler(Timer timer) {
    unsigned int timerBaseAddr;
    switch (timer) {
        case TIMER2: timerBaseAddr = SOC_DMTIMER_2_REGS; break;
        case TIMER3: timerBaseAddr = SOC_DMTIMER_3_REGS; break;
        case TIMER4: timerBaseAddr = SOC_DMTIMER_4_REGS; break;
        case TIMER5: timerBaseAddr = SOC_DMTIMER_5_REGS; break;
        case TIMER6: timerBaseAddr = SOC_DMTIMER_6_REGS; break;
        case TIMER7: timerBaseAddr = SOC_DMTIMER_7_REGS; break;
        default: return;
    }

    HWREG(timerBaseAddr + DMTIMER_IRQSTATUS) = 0x2;
    flag_timer = true;
    timerDisable(timer);
}

void delay(unsigned int count, Timer timer) {
    unsigned int timerBaseAddr;
    switch (timer) {
        case TIMER2: timerBaseAddr = SOC_DMTIMER_2_REGS; break;
        case TIMER3: timerBaseAddr = SOC_DMTIMER_3_REGS; break;
        case TIMER4: timerBaseAddr = SOC_DMTIMER_4_REGS; break;
        case TIMER5: timerBaseAddr = SOC_DMTIMER_5_REGS; break;
        case TIMER6: timerBaseAddr = SOC_DMTIMER_6_REGS; break;
        case TIMER7: timerBaseAddr = SOC_DMTIMER_7_REGS; break;
        default: return;
    }

    while (count != 0) {
        DMTimerWaitForWrite(0x2, timer);

        HWREG(timerBaseAddr + DMTIMER_TCRR) = 0x0;
        timerEnable(timer);

        while (HWREG(timerBaseAddr + DMTIMER_TCRR) < TIMER_1MS_COUNT);

        timerDisable(timer);
        count--;
    }
}

unsigned int timerRead(Timer timer) {
    unsigned int timerBaseAddr;
    switch (timer) {
        case TIMER2: timerBaseAddr = SOC_DMTIMER_2_REGS; break;
        case TIMER3: timerBaseAddr = SOC_DMTIMER_3_REGS; break;
        case TIMER4: timerBaseAddr = SOC_DMTIMER_4_REGS; break;
        case TIMER5: timerBaseAddr = SOC_DMTIMER_5_REGS; break;
        case TIMER6: timerBaseAddr = SOC_DMTIMER_6_REGS; break;
        case TIMER7: timerBaseAddr = SOC_DMTIMER_7_REGS; break;
        default: return 0;
    }
    return HWREG(timerBaseAddr + DMTIMER_TCRR);
}

void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}