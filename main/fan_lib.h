#ifndef FAN_H
#define FAN_H

#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/ledc.h"

#define PWM_RESOLUTION LEDC_TIMER_10_BIT
// #define LEDC_HIGH_SPEED_MODE LEDC_HIGH_SPEED_MODE
// #define LEDC_LOW_SPEED_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_0 LEDC_CHANNEL_0
#define PWM_FREQ 5000
#define DUTY_OFF 0
#define DUTY_LIGHT 400
#define DUTY_MEDIUM 200
#define DUTY_HIGH  50

void set_fan_gpio(int gpio1, int gpio2);
void set_motor_speed(int gpio1, int gpio2, int state);
int getStateFan();

#endif 