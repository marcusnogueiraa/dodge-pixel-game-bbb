#include "game.h"
#include "uart.h"

// Definição das variáveis globais
EntityGpio leftButton = {GPIO1, 13, CM_conf_gpmc_ad13}; // P8-11
EntityGpio rightButton = {GPIO1, 14, CM_conf_gpmc_ad14}; // P8-16
EntityGpio buzzer = {GPIO1, 28, CM_conf_gpmc_ben1}; // P9-12

EntityGpio pixelMapping[4][2] = {
    //          P8-18                             P9-15
    { {GPIO2, 1, CM_conf_gpmc_clk}, {GPIO1, 16, CM_conf_gpmc_a0} },
    //          P8-38                             P9-23
    { {GPIO2, 15, CM_conf_lcd_data9}, {GPIO1, 17, CM_conf_gpmc_a1} },
    //          P8-42                             P9-14
    { {GPIO2, 11, CM_conf_lcd_data5}, {GPIO1, 18, CM_conf_gpmc_a2} },
    //          P8-45                             P9-16
    { {GPIO2, 6, CM_conf_lcd_data0}, {GPIO1, 19, CM_conf_gpmc_a3} }
};

ENTITY grid[GRID_HEIGHT][GRID_WIDTH] = {
    {EMPTY, EMPTY},
    {EMPTY, EMPTY},
    {EMPTY, EMPTY},
    {EMPTY, PLAYER}
};

unsigned int seed = 1;
bool isPlaying = false;
bool blockIsr = false;

void move_left(void) {
    if (blockIsr) {putCh(UART0, 'a'); return;}
    if (!isPlaying) {putCh(UART0, 'b'); return;}
    if (!is_empty(3, 0)) {putCh(UART0, 'c'); return;}
    
    grid[3][RIGHT_COLUMN] = EMPTY;
    grid[3][LEFT_COLUMN] = PLAYER;
}

void move_right(void) {
    if (blockIsr) {putCh(UART0, 'a'); return;}
    if (!isPlaying) {putCh(UART0, 'b'); return;}
    if (!is_empty(3, 1)) {putCh(UART0, 'c'); return;}
    
    grid[3][LEFT_COLUMN] = EMPTY;
    grid[3][RIGHT_COLUMN] = PLAYER;
}

bool is_empty(int i, int j) {
    return grid[i][j] == EMPTY;
}

bool is_obstacle(int i, int j) {
    return grid[i][j] == OBSTACLE;
}

bool verify_colision(unsigned char column){
    return grid[3][column] == PLAYER && grid[2][column] == OBSTACLE; 
}

void knock_animation(int column){
    for(int i = 0; i < 3; i++){
        knock_song();
        set_pixel_status(3, column, HIGH);
        set_pixel_status(2, column, HIGH);
        delay(200, TIMER7);
        set_pixel_status(3, column, LOW);
        set_pixel_status(2, column, LOW);
        delay(200, TIMER7);
    }
}

void knock_song(){
    GpioSetPinValue(buzzer.gpioMod, buzzer.pinNumber, HIGH);
    delay(50, TIMER7);
    GpioSetPinValue(buzzer.gpioMod, buzzer.pinNumber, LOW);
    delay(50, TIMER7);

    GpioSetPinValue(buzzer.gpioMod, buzzer.pinNumber, HIGH);
    delay(50, TIMER7);
    GpioSetPinValue(buzzer.gpioMod, buzzer.pinNumber, LOW);
    delay(50, TIMER7);
}

void update_state(STATE *state) {
    switch (*state) {
        case STATE_0:
            *state = STATE_1;
            break;
        case STATE_1:
            *state = STATE_2;
            break;
        case STATE_2:
            *state = STATE_3;
            break;
        case STATE_3:
            *state = STATE_0;
            break;
        default:
            *state = STATE_0;
            break;
    }
}

void handle_state(STATE *state) {
    if (*state == STATE_0) {
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

        if (*state == STATE_3 && verify_colision(obstacle_j)){
            putString(UART0, "[LOG] Game Over\n\r", 17);
            blockIsr = true;
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

