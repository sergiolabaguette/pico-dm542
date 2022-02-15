#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

enum Setup {
    /* GPIO pins, BCM */
    PUL = 14,
    DIR = 10,
    ENA = 0,
    /* Delay in us, i.e. 1ms = 1000 us. Doesn't work under 300 */
    DELAY = 300,
    /* Number of steps, 400 steps for full rotation */
    STEPS = 10000,
};

static void setup_pins(int pin1, int pin2, int pin3) {
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

static void step_once(int stepGPIO, int delay) {
    gpio_put(stepGPIO, 1);
    /* busy_wait is faster than sleep() */
    busy_wait_us(delay);
    gpio_put(stepGPIO, 0);
    busy_wait_us(delay);
}

static void set_direction(int direction){
    /* 0 for UP, 1 for DOWN */
    enum Setup gpio_pin_dir = DIR;
    gpio_put(gpio_pin_dir, direction);
    busy_wait_ms(500);
}

static void run (int gpio_pin_ena, int gpio_pin_pul, int step_delay, int steps_to_move){
    /* false enables driver, true disables driver */
    gpio_put(gpio_pin_ena, 0);
    /* DIR signal must be ahead of PUL by 500 us to ensure correct direction */
    for (size_t i = 0; i < steps_to_move; ++i){
        step_once(gpio_pin_pul, step_delay);
    };
    /* turn off the driver after every run to reduce heat */
    gpio_put(gpio_pin_ena, 1);
}
int main() {
    /* direction 1 for UP, 0 for DOWN */
    enum Setup step_delay = DELAY;
    enum Setup step_count = STEPS;
    enum Setup gpio_pin_pul = PUL;
    enum Setup gpio_pin_dir = DIR;
    enum Setup gpio_pin_ena = ENA;

    setup_pins(gpio_pin_pul, gpio_pin_dir, gpio_pin_ena);

    set_direction(1);
    run(gpio_pin_ena, gpio_pin_pul, step_delay, 24000);


    for (size_t j = 0; j < 5; ++j){
        set_direction(0);
        run(gpio_pin_ena, gpio_pin_pul, step_delay, step_count);
        set_direction(1);
        run(gpio_pin_ena, gpio_pin_pul, step_delay, step_count);
    }


    return 0;
}
