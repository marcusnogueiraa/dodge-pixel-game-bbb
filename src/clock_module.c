#include "clock_module.h"
#include "soc_AM335x.h"
#include "hw_types.h"

void ckmSetCLKModuleRegister(CKM_MODULE_REG module, unsigned int value){
    HWREG(module + SOC_CONTROL_REGS)|= value;
}
unsigned int ckmGetCLKModuleRegister(CKM_MODULE_REG module){
    return HWREG(module);
}
