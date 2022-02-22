#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

enum Setup {
    /* GPIO pins, BCM */
    PUL = 0,
    DIR = 10,
    ENA = 14,
    BTN_UP = 5,
    BTN_DOWN = 3,
    SPDT_TOP = 16,
    /* Delay in us, i.e. 1ms = 1000 us. Doesn't work under 280 */
    DELAY = 290,
    /* Number of steps, 400 steps for full rotation */
    STEPS = 30000,

};

int up_flag = 0, down_flag = 0, moving_flag = 0;

static void setup_pins_out(int pin1, int pin2, int pin3) {
    gpio_init(pin1);
    gpio_init(pin2);
    gpio_init(pin3);
    /* sets direction to OUT, false sets IN */
    gpio_set_dir(pin1, 1);
    gpio_set_dir(pin2, 1);
    gpio_set_dir(pin3, 1);
    /* sets gpio to pull-up */
    gpio_pull_up(pin1);
    gpio_pull_up(pin2);
    gpio_pull_up(pin3);
}

static void setup_pins_in(int pin1, int pin2) {
    gpio_init(pin1);
    gpio_init(pin2);
    /* set gpio input */
    gpio_set_dir(pin1, 0);
    gpio_set_dir(pin2, 0);
    /* enable pull-up resistor */
    gpio_pull_up(pin1);
    gpio_pull_up(pin2);
}

static void step_once(int stepGPIO, int delay) {
    gpio_put(stepGPIO, 1);
    /* busy_wait is faster than sleep() */
    sleep_us(delay);
    gpio_put(stepGPIO, 0);
    sleep_us(delay);
}

static int set_direction(int direction){
    /* 1 for UP, 0 for DOWN */
    enum Setup gpio_pin_dir = DIR;
    gpio_put(gpio_pin_dir, direction);
    /* DIR signal must be ahead of PUL by 50 ms to ensure correct direction */
    sleep_ms(50);

    return 0;
}

static int full_rotation (int gpio_pin_ena, int gpio_pin_pul, int step_delay, int steps_to_move, int direction){
    set_direction(direction);
    /* false enables driver, true disables driver */
    gpio_put(gpio_pin_ena, 0);
    for (size_t i = 0; i < steps_to_move; ++i){
        step_once(gpio_pin_pul, step_delay);
    };
    /* turn off the driver after every run to reduce heat */
    gpio_put(gpio_pin_ena, 1);

    return 0;
}

static void handle_input(int gpio_pin_ena, int gpio_pin_pul, int step_delay, int step_count, int pin_up, int pin_down){

    if (moving_flag == false){
        if( gpio_get(pin_up) == false && up_flag == false && moving_flag == false) {
            moving_flag = true;
            down_flag = false;
            full_rotation(gpio_pin_ena,gpio_pin_pul,step_delay,step_count,1);
            busy_wait_ms(50);
            up_flag = true;
            moving_flag = false;            
        }

        if( gpio_get(pin_down) == false && down_flag == false && moving_flag == false ) {
            moving_flag = true;
            up_flag = false;
            full_rotation(gpio_pin_ena,gpio_pin_pul,step_delay,step_count,0);
            busy_wait_ms(50);
            down_flag = true;
            moving_flag = false;
        }        
    }
}

int main() {
    enum Setup step_delay = DELAY;
    enum Setup step_count = STEPS;
    enum Setup gpio_pin_pul = PUL;
    enum Setup gpio_pin_dir = DIR;
    enum Setup gpio_pin_ena = ENA;
    enum Setup btn_pin_up = BTN_UP;
    enum Setup btn_pin_down = BTN_DOWN;
    enum Setup top_switch = SPDT_TOP;

    /* SPDT test */
    gpio_init(top_switch);
    gpio_set_dir(top_switch, 0);
    gpio_pull_up(top_switch);
    int led = 25;
    gpio_init(led);
    gpio_set_dir(led, 1);

    /* always run setup_pins before anything else */
    setup_pins_out(gpio_pin_pul, gpio_pin_dir, gpio_pin_ena);
    setup_pins_in(btn_pin_up, btn_pin_down);

    /* button logic */
    while(1){
        /*handle_input(gpio_pin_ena, gpio_pin_pul, step_delay, step_count, btn_pin_up, btn_pin_down);*/
        if(gpio_get(top_switch) == 0){
            gpio_put(led, 0);
        } else {
            gpio_put(led, 1);
        }
    }
   
    return 0;
}
