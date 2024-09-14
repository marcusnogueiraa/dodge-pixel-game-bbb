#include "gpio.h"
#include "clock_module.h"
#define TOGGLE 0x01u

int Init_module_gpio(gpioMod module){

    unsigned int val_temp = (1<<18) | (1<<2);
    switch (module)
    {
    case GPIO1:
        ckmSetCLKModuleRegister(CKM_PER_GPIO1_CLKCTRL, val_temp);
        break;
    case GPIO2:
        ckmSetCLKModuleRegister(CKM_PER_GPIO2_CLKCTRL, val_temp);
        break;
    case GPIO3:
        ckmSetCLKModuleRegister(CKM_PER_GPIO3_CLKCTRL, val_temp);
        break;        
    default:
        break;
    }

}

int Init_pin_gpio(gpioMod module , unsigned char pin , Direction control){

    unsigned int val_temp = 0;
    switch (module)
    {
    case GPIO1:
        val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
        break;
    case GPIO2:
        val_temp = HWREG(SOC_GPIO_2_REGS+GPIO_OE);
        break;
    case GPIO3:
        val_temp = HWREG(SOC_GPIO_3_REGS+GPIO_OE);
        break;        
    default:
        break;
    }

    if(control == OUTPUT){
        val_temp &= ~(1<<pin);
    }else{
        val_temp |= (1<<pin);
    }
    
    switch (module)
    {
    case GPIO1:
        HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;
        break;
    case GPIO2:
        HWREG(SOC_GPIO_2_REGS+GPIO_OE) = val_temp;
        break;
    case GPIO3:
        HWREG(SOC_GPIO_3_REGS+GPIO_OE) = val_temp;
        break;
    default:
        break;
    }

}

static void Set_direction_pin_gpio(gpioMod module, Direction control, unsigned char pin){


    unsigned int val_temp = 0;
    switch (module)
    {
    case GPIO1:
        val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
        break;
    case GPIO2:
        val_temp = HWREG(SOC_GPIO_2_REGS+GPIO_OE);
        break;
    case GPIO3:
        val_temp = HWREG(SOC_GPIO_3_REGS+GPIO_OE);
        break;        
    default:
        break;
    }

    if(control == OUTPUT){
        val_temp &= ~(1<<pin);
    }else{
        val_temp |= (1<<pin);
    }
    
    switch (module)
    {
    case GPIO1:
        HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;
        break;
    case GPIO2:
        HWREG(SOC_GPIO_2_REGS+GPIO_OE) = val_temp;
        break;
    case GPIO3:
        HWREG(SOC_GPIO_3_REGS+GPIO_OE) = val_temp;
        break;
    default:
        break;
    }    
}

unsigned int Get_direction_pin_gpio(gpioMod module , unsigned char pin){
    
        unsigned int val_temp = 0;
        switch (module)
        {
        case GPIO1:
            val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
            break;
        case GPIO2:
            val_temp = HWREG(SOC_GPIO_2_REGS+GPIO_OE);
            break;
        case GPIO3:
            val_temp = HWREG(SOC_GPIO_3_REGS+GPIO_OE);
            break;        
        default:
            break;
        }
    
        if((val_temp & (1<<pin)) != 0){
            return INPUT;
        }else{
            return OUTPUT;
        }
}

void GpioSetPinValue(gpioMod module , ucPinNumber pin, Level state){

    switch (module)
    {
    case GPIO1:
        if(state == HIGH){
            HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<pin);
        }else{
            HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<pin);
        }
        break;
    case GPIO2:
        if(state == HIGH){
            HWREG(SOC_GPIO_2_REGS+GPIO_SETDATAOUT) = (1<<pin);
        }else{
            HWREG(SOC_GPIO_2_REGS+GPIO_CLEARDATAOUT) = (1<<pin);
        }
        break;
    case GPIO3:
        if(state == HIGH){
            HWREG(SOC_GPIO_3_REGS+GPIO_SETDATAOUT) = (1<<pin);
        }else{
            HWREG(SOC_GPIO_3_REGS+GPIO_CLEARDATAOUT) = (1<<pin);
        }
        break;            
    default:
        break;
    }
}

unsigned int GpioGetPinValue(gpioMod module, ucPinNumber pin){

    switch (module)
    {
    case GPIO1:
        if(((HWREG(SOC_GPIO_1_REGS+GPIO_DATAIN) & (1<<pin))) != 0){
		    return HIGH;
	    }else{
            return LOW;  
        }    
        break;
    case GPIO2:
        if(((HWREG(SOC_GPIO_2_REGS+GPIO_DATAIN) & (1<<pin))) != 0){
            return HIGH;
        }else{
            return LOW;
        }    
        break;
    case GPIO3:
        if(((HWREG(SOC_GPIO_3_REGS+GPIO_DATAIN) & (1<<pin))) != 0){
            return HIGH;
        }else{
            return LOW;  
        }    
        break;        
    
    default:
        return LOW; 
    }
}

int gpioIsrHandler(gpioMod mod, GroupInterrup type, ucPinNumber pin) {
    unsigned int baseAddr;
    unsigned int irqStatusOffset;

    switch (mod) {
        case GPIO0:
            baseAddr = SOC_GPIO_0_REGS;
            break;
        case GPIO1:
            baseAddr = SOC_GPIO_1_REGS;
            break;
        case GPIO2:
            baseAddr = SOC_GPIO_2_REGS;
            break;
        case GPIO3:
            baseAddr = SOC_GPIO_3_REGS;
            break;
        default:
            return -1;
    }

    irqStatusOffset = (type == type0) ? GPIO_IRQSTATUS_0 : GPIO_IRQSTATUS_1;

    HWREG(baseAddr + irqStatusOffset) |= (1 << pin);

    return 1;
}

int Pin_Interrup_Config(gpioMod mod, ucPinNumber pin, GroupInterrup type) {
    unsigned int baseRegs[] = {
        SOC_GPIO_0_REGS,
        SOC_GPIO_1_REGS,
        SOC_GPIO_2_REGS,
        SOC_GPIO_3_REGS
    };
    
    if (mod < GPIO0 || mod > GPIO3) return -1;
    if (pin >= 32) return -1;
    
    unsigned int base = baseRegs[mod]; 
    
    unsigned int irqStatusSet = base + (type == type0 ? GPIO_IRQSTATUS_SET_0 : GPIO_IRQSTATUS_SET_1);
    unsigned int risingDetect = base + GPIO_RISINGDETECT;
    
    HWREG(irqStatusSet) |= (1 << pin);
    HWREG(risingDetect) |= (1 << pin);

    return 1;
}
