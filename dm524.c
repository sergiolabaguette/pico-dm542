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
const int STEP_DELAY = 300;
const int STEPS_PER_REV = 800;
// 1 for LEFT (UP), 0 for RIGHT (DOWN)
int rotation_direction = 0;


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
    }
}

static void single_step(){
    gpio_put(*p_PUL, true);
    busy_wait_us(*p_DELAY);
    gpio_put(*p_PUL, false);
    busy_wait_us(*p_DELAY);
}

static void full_revolution(){
    int i;
    for(i = 0; i < *p_STEPS; ++i){
        single_step();
    }
}

static void set_direction(){
    busy_wait_ms(25);
    gpio_put(*p_DIR, *p_DIRECTION);
    busy_wait_ms(25);
}

int main(){
    int i = 0;
    int revolutions = 4;
    setup_driver_pins();
    set_direction();
    while(i < revolutions){
        full_revolution();
        rotation_direction = !rotation_direction;
        set_direction();
        ++i;
    }
    return 0;
}