#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// TODO

// poprawic easing (!!!)

// dodac limitery
// ogarnac stan
// reboot on error 


// DRIVER PINS
const uint GPIO_PUL = 19;
const uint GPIO_DIR = 20;
const uint GPIO_ENA = 21;
// BUTTONS
const uint GPIO_BTN_TOP = 2;
const uint GPIO_BTN_BOT = 6;
const uint GPIO_LIMIT_TOP = 10;
const uint GPIO_LIMIT_BOT = 14;

const uint STEPS_PER_REV = 1600;
// dont go under 300/no-microstepping
static uint STEP_DELAY = 100;

//
// ROTATION STATE
// 1 for LEFT (UP), 0 for RIGHT (DOWN)
//

uint ease_arr[] = { 7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,6000,6000,6000,5000,5000,5000,5000,4286,4286,4286,4286,4286,3750,3750,3750,3333,3333,3333,3333,3000,3000,3000,3000,3000,2727,2727,2727,2500,2500,2500,2500,2308,2308,2308,2308,2143,2143,2143,2143,2143,2000,2000,2000,2000,1875,1875,1875,1765,1765,1765,1765,1765,1667,1667,1667,1667,1579,1579,1579,1500,1500,1500,1500,1500,1429,1429,1429,1429,1364,1364,1364,1304,1304,1304,1304,1304,1250,1250,1250,1250,1200,1200,1200,1154,1154,1154,1154,1111,1111,1111,1111,1071,1071,1071,1071,1071,1034,1034,1034,1034,1000,1000,1000,1000,968,968,968,938,938,938,938,909,909,909,909,882,882,882,882,882,857,857,857,857,833,833,833,833,811,811,811,789,789,789,789,769,769,769,769,750,750,750,750,732,732,732,732,732,714,714,714,714,698,698,698,698,682,682,682,667,667,667,667,652,652,652,652,638,638,638,638,638,625,625,625,625,612,612,612,612,600,600,600,588,588,588,588,577,577,577,577,566,566,566,566,556,556,556,556,545,545,545,545,536,536,536,536,536,526,526,526,526,517,517,517,517,508,508,508,508,500,500,500,500,492,492,492,492,484,484,484,476,476,476,476,469,469,469,469,462,462,462,462,455,455,455,455,448,448,448,448,441,441,441,441,435,435,435,435,435,429,429,429,429,423,423,423,423,417,417,417,417,411,411,411,411,405,405,405,405,400,400,400,395,395,395,395,390,390,390,390,385,385,385,385,380,380,380,380,375,375,375,375,370,370,370,370,370,366,366,366,366,361,361,361,361,357,357,357,357,353,353,353,353,349,349,349,349,345,345,345,341,341,341,341,337,337,337,337,333,333,333,333,330,330,330,330,326,326,326,326,323,323,323,323,319,319,319,319,319,316,316,316,316,313,313,313,313,309,309,309,309,306,306,306,306,303,303,200,200,200,200};
// set initial state to 0 (DOWN), 1 is UP

const int *p_DELAY = &STEP_DELAY;
const int *p_PUL = &GPIO_PUL;
const int *p_DIR = &GPIO_DIR;
const int *p_ENA = &GPIO_ENA;

const int *p_BT = &GPIO_BTN_TOP;
const int *p_BB = &GPIO_BTN_BOT;
const int *p_LT = &GPIO_LIMIT_TOP;
const int *p_LB = &GPIO_LIMIT_BOT;

const int *p_STEPS = &STEPS_PER_REV;

static void setup_pins(int pin_arr[], uint arr_size, bool pin_direction){    
    for(size_t i = 0; i < arr_size; ++i){
        gpio_init(pin_arr[i]);
        gpio_set_dir(pin_arr[i], pin_direction);
        gpio_pull_up(pin_arr[i]);
    };
};

static void single_step(){
    gpio_put(*p_PUL, true);
    busy_wait_us(*p_DELAY);
    gpio_put(*p_PUL, false);
    busy_wait_us(*p_DELAY);
};

static void full_revolution(){
    for(size_t i = 0; i < *p_STEPS; i++){
        single_step();
    };
};

static void set_direction(bool direction){
    // @direction --> true is UP, false is DOWN
    busy_wait_ms(25);
    gpio_put(*p_DIR, direction);
    busy_wait_ms(25);
};

static void ease_in(){
    for(size_t m = 0; m < 400; m++){
        STEP_DELAY = (int)(ease_arr[m] / 2);
        single_step();
        single_step();
        single_step();
        single_step();
    };
};

static void ease_out(){
    for (int j = 399; j >= 0; j--){    
        STEP_DELAY = (int)(ease_arr[j] / 2);
        single_step();
        single_step();
        single_step();
        single_step();
    };
};

static void enable_driver(){
    busy_wait_ms(100);
    gpio_put(*p_ENA, false);
    busy_wait_ms(100);
};

static void disable_driver(){
    busy_wait_ms(100);
    gpio_put(*p_ENA, true);
    busy_wait_ms(100);
};

static void travel(uint rotations, bool direction){
    enable_driver();
    set_direction(direction);
    ease_in();
    for (size_t k = 0; k < rotations; k++){
        full_revolution();
    };
    ease_out();
    disable_driver();
};

int main() {

    int button_pins[] = { *p_BT, *p_BB, *p_LT, *p_LB };
    int driver_gpio_pins[] = { *p_PUL, *p_DIR, *p_ENA };

    uint button_pins_size = sizeof(button_pins) / sizeof(button_pins[0]);
    uint driver_pins_size = sizeof(driver_gpio_pins) / sizeof(driver_gpio_pins[0]);

    stdio_init_all();
    setup_pins(button_pins, button_pins_size, false);
    setup_pins(driver_gpio_pins, driver_pins_size, true);
    
    // button infinite loop
    while(1){
        if(gpio_get(*p_BB) != 0 && gpio_get(*p_BT) == 0){
            travel(10,false);
            busy_wait_ms(100);
        };
        if(gpio_get(*p_BT) != 0 && gpio_get(*p_BB) == 0){
            travel(10,true);
            busy_wait_ms(100);
        };
        // no-op function for hw tight loops
        tight_loop_contents();
    };

    return 0;
};