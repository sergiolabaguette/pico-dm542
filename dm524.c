#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// TODO
// dodac guziki
// dodac limitery
// ogarnac stan
// reboot on error 


// DRIVER PINS
const int GPIO_PUL = 19;
const int GPIO_DIR = 20;
const int GPIO_ENA = 21;
// BUTTONS
const int GPIO_BTN_TOP = 2;
const int GPIO_BTN_BOT = 6;
const int GPIO_LIMIT_TOP = 10;
const int GPIO_LIMIT_BOT = 14;

const int STEPS_PER_REV = 400;
// dont go under 300
static int STEP_DELAY = 300;
// 1 for LEFT (UP), 0 for RIGHT (DOWN)
bool rotation_direction;

int ease_arr[] = { 30000,25000,20000,15000,14000,14000,12000,12000,12000,9000,9000,9000,9000,7500,7500,7500,7500,7500,6000,6000,6000,5000,5000,5000,5000,4286,4286,4286,4286,4286,3750,3750,3750,3333,3333,3333,3333,3000,3000,3000,3000,3000,2727,2727,2727,2500,2500,2500,2500,2308,2308,2308,2308,2143,2143,2143,2143,2143,2000,2000,2000,2000,1875,1875,1875,1765,1765,1765,1765,1765,1667,1667,1667,1667,1579,1579,1579,1500,1500,1500,1500,1500,1429,1429,1429,1429,1364,1364,1364,1304,1304,1304,1304,1304,1250,1250,1250,1250,1200,1200,1200,1154,1154,1154,1154,1111,1111,1111,1111,1071,1071,1071,1071,1071,1034,1034,1034,1034,1000,1000,1000,1000,968,968,968,938,938,938,938,909,909,909,909,882,882,882,882,882,857,857,857,857,833,833,833,833,811,811,811,789,789,789,789,769,769,769,769,750,750,750,750,732,732,732,732,732,714,714,714,714,698,698,698,698,682,682,682,667,667,667,667,652,652,652,652,638,638,638,638,638,625,625,625,625,612,612,612,612,600,600,600,588,588,588,588,577,577,577,577,566,566,566,566,556,556,556,556,545,545,545,545,536,536,536,536,536,526,526,526,526,517,517,517,517,508,508,508,508,500,500,500,500,492,492,492,492,484,484,484,476,476,476,476,469,469,469,469,462,462,462,462,455,455,455,455,448,448,448,448,441,441,441,441,435,435,435,435,435,429,429,429,429,423,423,423,423,417,417,417,417,411,411,411,411,405,405,405,405,400,400,400,395,395,395,395,390,390,390,390,385,385,385,385,380,380,380,380,375,375,375,375,370,370,370,370,370,366,366,366,366,361,361,361,361,357,357,357,357,353,353,353,353,349,349,349,349,345,345,345,341,341,341,341,337,337,337,337,333,333,333,333,330,330,330,330,326,326,326,326,323,323,323,323,319,319,319,319,319,316,316,316,316,313,313,313,313,309,309,309,309,306,306,306,306,303,303,303,303,300,300};
// set initial state to 0 (DOWN), 1 is UP;
static int lift_state = 1;

const int *p_DELAY = &STEP_DELAY;
const int *p_PUL = &GPIO_PUL;
const int *p_DIR = &GPIO_DIR;
const int *p_ENA = &GPIO_ENA;

const int *p_BT = &GPIO_BTN_TOP;
const int *p_BB = &GPIO_BTN_BOT;
const int *p_LT = &GPIO_LIMIT_TOP;
const int *p_LB = &GPIO_LIMIT_BOT;

const int *p_STEPS = &STEPS_PER_REV;

static void setup_button_pins(){
    int button_pins[] = { *p_BT, *p_BB, *p_LT, *p_LB };
    size_t n = sizeof button_pins / sizeof *button_pins;
    for (size_t i = 0; i < n; ++i){
        gpio_init(button_pins[i]);
        gpio_set_dir(button_pins[i], false);
        gpio_pull_up(button_pins[i]);
    };
};

static void setup_driver_pins(){
    int driver_gpio_pins[] = { *p_PUL, *p_DIR, *p_ENA };    
    size_t n = sizeof driver_gpio_pins / sizeof *driver_gpio_pins;
    for(size_t i = 0; i < n; ++i){
        gpio_init(driver_gpio_pins[i]);
        gpio_set_dir(driver_gpio_pins[i], true);
        gpio_pull_up(driver_gpio_pins[i]);
    };
};

static void single_step(){
    gpio_put(*p_PUL, true);
    busy_wait_us(*p_DELAY);
    gpio_put(*p_PUL, false);
    busy_wait_us(*p_DELAY);
};

static void full_revolution(){
    for(size_t i = 0; i < *p_STEPS; ++i){
        single_step();
    };
};

static void set_direction(bool direction){    
    rotation_direction = direction;
    busy_wait_ms(25);
    gpio_put(*p_DIR, rotation_direction);
    busy_wait_ms(25);
};

static void ease_in(){
    for(size_t i = 0; i < *p_STEPS; ++i){
        STEP_DELAY = ease_arr[i];
        single_step();
    };

    STEP_DELAY = 300;
};

static void ease_out(){
    for (int j = 399; j > 0; --j){    
        STEP_DELAY = ease_arr[j];
        single_step();
    }; 
};

static void travel(int rotations, bool direction){
    set_direction(direction);
    ease_in();
    for (size_t k = 0; k < rotations; ++k){
        full_revolution();
    };
    ease_out();
};

int main() {

    stdio_init_all();
    setup_driver_pins();
    for (size_t g = 0; g < 10; g++)
    {
        travel(55,false);
        busy_wait_ms(200);
        travel(55, true);
        busy_wait_ms(200);
    };
    return 0;
};