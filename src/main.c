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
void knock_animation();
void reset_grid();
void match_tasks(STATE *state);
unsigned int random_number();
unsigned char get_random_column();
bool verify_colision(unsigned char column);

unsigned int seed = 1;
bool firstTimePlaying = false;
bool blockIsr = false;

/*-----------------------------------------------------------------------------
*  Main Function
*-----------------------------------------------------------------------------*/
int main(void){
	disableWdt();

	timerSetup(TIMER7);
    timerEnable(TIMER7);
	
    setupGpio();

    Interrupt_Setup(95);
    Interrupt_Setup(98);
    Pin_Interrup_Config(leftButton.gpioMod, leftButton.pinNumber, type0);
    Pin_Interrup_Config(rightButton.gpioMod, rightButton.pinNumber, type0);

    unsigned int delayInMs = 200;

    putString(UART0, "[LOG] Inicializando\n\r", 21);   
    
    while(true){
        idle_mode();
        isPlaying = true;
        
        STATE current_state = GENERATE_STATE;
        putString(UART0, "[LOG] Jogo Iniciado\n\r", 21);
        while(isPlaying){
            match_tasks(&current_state);
            delay(delayInMs, TIMER7);
        }
    }

	return 0;
}

unsigned int random_number(){
    unsigned int x = seed;
    x ^= (x << 13);
    x ^= (x >> 17);
    x ^= (x << 5);
    seed = x;
    return seed;
}

unsigned char get_random_column(){
    return random_number() % 2;
}

void idle_mode(){
    putString(UART0, "[LOG] Estado Ocioso\n\r", 21);

    reset_grid();
    render_frame();
    log_grid();

    while(!GpioGetPinValue(leftButton.gpioMod, leftButton.pinNumber) 
            && !GpioGetPinValue(rightButton.gpioMod, rightButton.pinNumber));
    
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

void handle_state(STATE *state) {
    if (*state == GENERATE_STATE) {
        // Gera um obstáculo em uma coluna pseudo aleatória
        unsigned char obstacle_j = get_random_column();
        grid[0][obstacle_j] = OBSTACLE;

        // Desaparece com os obstáculos da margem inferior
        if (is_obstacle(PLAYER_LINE, LEFT_COLUMN)) grid[PLAYER_LINE][LEFT_COLUMN] = EMPTY;
        else if (is_obstacle(PLAYER_LINE, RIGHT_COLUMN)) grid[PLAYER_LINE][RIGHT_COLUMN] = EMPTY;
        
        // Atualiza o estado para o próximo
        update_state(state);
    } else {
        // Verifica qual coluna o obstáculo está
        unsigned char obstacle_j;
        if (is_obstacle(*state - 1, LEFT_COLUMN)) obstacle_j = LEFT_COLUMN;
        else obstacle_j = RIGHT_COLUMN;

        if (*state == DOWN_OBSTACLES_3 && verify_colision(obstacle_j)){
            blockIsr = true;
            putString(UART0, "[LOG] Game Over\n\r", 17);
            knock_animation(obstacle_j);
            isPlaying = false;
            blockIsr = false;
            return;
        } 

        // Movimenta o obstáculo para baixo
        grid[*state - 1][obstacle_j] = EMPTY;
        grid[*state][obstacle_j] = OBSTACLE;

        // Atualiza o estado para o próximo
        update_state(state);
    }
}

void knock_animation(int column){
    for(int i = 0; i < 3; i++){
        set_pixel_status(3, column, HIGH);
        set_pixel_status(2, column, HIGH);
        delay(200, TIMER7);
        set_pixel_status(3, column, LOW);
        set_pixel_status(2, column, LOW);
        delay(200, TIMER7);
    }
}

bool verify_colision(unsigned char column){
    return grid[3][column] == PLAYER && grid[2][column] == OBSTACLE; 
}

void update_state(STATE *state) {
    // Transição cíclica entre estados
    switch (*state) {
        case GENERATE_STATE:
            *state = DOWN_OBSTACLES_1;
            break;
        case DOWN_OBSTACLES_1:
            *state = DOWN_OBSTACLES_2;
            break;
        case DOWN_OBSTACLES_2:
            *state = DOWN_OBSTACLES_3;
            break;
        case DOWN_OBSTACLES_3:
            *state = GENERATE_STATE;
            break;
        default:
            // Estado inválido, reinicia para GENERATE_STATE
            *state = GENERATE_STATE;
            break;
    }
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

void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}

void ISR_Handler(void) {
    //if (blockIsr) return;
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