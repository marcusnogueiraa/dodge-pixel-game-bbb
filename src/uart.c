
#include "pad.h"
#include "control_module.h"
#include "uart.h"
#include"clock_module.h"
#include"control_module.h"

static bool checkValidUart(UART_t uart){
   if((uart < UART0) || (uart > UART5)){
      // TODO: raise error (Uart number is either too big or negative: /uart)
      return(false);
   }
   return(true);
}
void uartInitModule(UART_t uart, unsigned int baudrate, STOP_BIT_t stopBit, PARITY_BIT_t parity, FLOW_t flowControl){
    if(checkValidUart(uart)){
        unsigned int uart_base =  UART_ARRAY_BASE[uart];

        // Configurações de clock e módulo UART
        unsigned int temp = ckmGetCLKModuleRegister(CKM_WKUP_UART0_CLKCTRL);
        temp &= ~(0b11);
        temp |= 0b10; // Habilita explicitamente o módulo
        ckmSetCLKModuleRegister(CKM_WKUP_UART0_CLKCTRL, temp);
        while((ckmGetCLKModuleRegister(CKM_WKUP_UART0_CLKCTRL) & (0b11<<16)) != 0);

        // Debug: Verifica se o clock foi configurado corretamente
        if((ckmGetCLKModuleRegister(CKM_WKUP_UART0_CLKCTRL) & (0b11<<16)) != 0) {
            GpioSetPinValue(GPIO1, 21, HIGH);  // Acende o LED no pino 21 se o clock não estiver configurado corretamente
            return;
        }

        // Configuração dos pinos
        cmSetCtrlModule(CM_conf_uart0_txd, 0); // UART0_tx
        cmSetCtrlModule(CM_conf_uart0_rxd, (1<<4)|(1<<5)); // UART0_rx
        padSetMode(CM_conf_uart0_txd, MODE_0); // p1.11 como UART0_tx
        padSetMode(CM_conf_uart0_rxd, MODE_0); // p1.10 como UART0_rx

        // Configura divisor de clock para a taxa de baud rate
        float div = 48000000.0 / (16.0 * (float)baudrate);
        unsigned int intdiv = (unsigned int) div;
        HWREG(uart_base + 0x0C) |= 0x80; // Divisor latch enable
        HWREG(uart_base + 0x00) = intdiv & 0xFF;  // DLL
        HWREG(uart_base + 0x04) = (intdiv >> 8) & 0x3F;  // DHL
        HWREG(uart_base + 0x0C) &= ~0x80; // Desabilita divisor latch
        
        // Debug: Verifica se o módulo UART foi configurado corretamente
        if((HWREG(uart_base + 0x14) & 0x40) != 0x40) {
            GpioSetPinValue(GPIO1, 22, HIGH);  // Acende o LED no pino 22 se houver problema na configuração do UART
            return;
        }
    }
}


void putCh(UART_t uart, char c){
   unsigned int uart_base = UART_ARRAY_BASE[uart];
   while((HWREG(uart_base+0x14)&0x20)!=0x20);   //wait until txfifo is empty
   HWREG(uart_base+0) = c;
}

char getCh(UART_t uart){
   unsigned int uart_base = UART_ARRAY_BASE[uart];
   while((HWREG(uart_base+0x14)&0x1)==0);     //wait for a character to be in the rx fifo
   return(HWREG(uart_base+0x0));
}

int putString(UART_t uart, char *str, unsigned int length){
   for(int i = 0; i < length; i++){
      putCh(uart,str[i]);
   }
   return(length);
}

int uartGetString(UART_t uart, char *buf, unsigned int length){
   for(int i = 0; i < length; i ++){
      buf[i] = getCh(uart);
      putCh(uart,buf[i]);
   }
   return(length);
}

void uartClearBuffer(UART_t uart){
   unsigned int uart_base = UART_ARRAY_BASE[uart];
   while((HWREG(uart_base+0x14)&0x1)==1)
      getCh(uart);
}
