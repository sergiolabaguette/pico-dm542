#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// TODO
// przetestowac to guwno
// dodac guziki
// dodac limitery
// ogarnac stan
// reboot on error 

const int GPIO_PUL = 19;
const int GPIO_DIR = 20;
const int GPIO_ENA = 21;
// dont go under 300
int STEP_DELAY = 300;
const int STEPS_PER_REV = 400;
// 1 for LEFT (UP), 0 for RIGHT (DOWN)
int rotation_direction = 0;

int ease_arr[] = 
    {
        30000,30000,30000,30000,30000,30000,15000,15000,15000,10000,10000,10000,10000,7500,7500,7500,7500,7500,6000,6000,6000,5000,5000,5000,5000,4286,4286,4286,4286,4286,3750,3750,3750,3333,3333,3333,3333,3000,3000,3000,3000,3000,2727,2727,2727,2500,2500,2500,2500,2308,2308,2308,2308,2143,2143,2143,2143,2143,2000,2000,2000,2000,1875,1875,1875,1765,1765,1765,1765,1765,1667,1667,1667,1667,1579,1579,1579,1500,1500,1500,1500,1500,1429,1429,1429,1429,1364,1364,1364,1304,1304,1304,1304,1304,1250,1250,1250,1250,1200,1200,1200,1154,1154,1154,1154,1111,1111,1111,1111,1071,1071,1071,1071,1071,1034,1034,1034,1034,1000,1000,1000,1000,968,968,968,938,938,938,938,909,909,909,909,882,882,882,882,882,857,857,857,857,833,833,833,833,811,811,811,789,789,789,789,769,769,769,769,750,750,750,750,732,732,732,732,732,714,714,714,714,698,698,698,698,682,682,682,667,667,667,667,652,652,652,652,638,638,638,638,638,625,625,625,625,612,612,612,612,600,600,600,588,588,588,588,577,577,577,577,566,566,566,566,556,556,556,556,545,545,545,545,536,536,536,536,536,526,526,526,526,517,517,517,517,508,508,508,508,500,500,500,500,492,492,492,492,484,484,484,476,476,476,476,469,469,469,469,462,462,462,462,455,455,455,455,448,448,448,448,441,441,441,441,435,435,435,435,435,429,429,429,429,423,423,423,423,417,417,417,417,411,411,411,411,405,405,405,405,400,400,400,395,395,395,395,390,390,390,390,385,385,385,385,380,380,380,380,375,375,375,375,370,370,370,370,370,366,366,366,366,361,361,361,361,357,357,357,357,353,353,353,353,349,349,349,349,345,345,345,341,341,341,341,337,337,337,337,333,333,333,333,330,330,330,330,326,326,326,326,323,323,323,323,319,319,319,319,319,316,316,316,316,313,313,313,313,309,309,309,309,306,306,306,306,303,303,303,303,300,300
    };

const int *p_DELAY = &STEP_DELAY;
const int *p_PUL = &GPIO_PUL;
const int *p_DIR = &GPIO_DIR;
const int *p_ENA = &GPIO_ENA;
const int *p_STEPS = &STEPS_PER_REV;
const int *p_DIRECTION = &rotation_direction;


void setup_driver_pins(){
    int driver_gpio_pins[] = {*p_PUL, *p_DIR, *p_ENA};
    int i;
    int n;
    n = sizeof driver_gpio_pins / sizeof *driver_gpio_pins;
    for(i = 0; i < n; ++i){
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
    int i;
    for(i = 0; i < *p_STEPS; ++i){
        single_step();
    };
};

static void set_direction(){
    busy_wait_ms(25);
    gpio_put(*p_DIR, *p_DIRECTION);
    busy_wait_ms(25);
};

static void ease_in(){
    int i;
    for(i = 0; i < 400; ++i){
        STEP_DELAY = ease_arr[i];
        single_step();
    };

    STEP_DELAY = 300;
};

int main(){
    setup_driver_pins();
    set_direction();
    ease_in();
    full_revolution();
    return 0;
};