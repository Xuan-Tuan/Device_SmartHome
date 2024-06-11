#include "led_lib.h"
#include "esp_log.h"
#include "driver/gpio.h"

int LEDgpio = GPIO_NUM_21;
static int LedState;

void setLEDgpio(int gpio){
    LEDgpio = gpio;
}
    
void led_set(int state){
    gpio_set_level(LEDgpio, state);
    gpio_set_direction(LEDgpio, GPIO_MODE_OUTPUT);
    LedState = state;
}
int getState(){
    return LedState;
}

    