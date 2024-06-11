#include "fan_lib.h"

    // Biến trạng thái hiện tại của quạt
static int current_fan_state = 0;
int fan_gpio1 = GPIO_NUM_13;
int fan_gpio2 = GPIO_NUM_14;

    // Hàm thiết lập GPIO cho quạt
void set_fan_gpio(int gpio1, int gpio2) {
    fan_gpio1 = gpio1;
    fan_gpio2 = gpio2;
}
    // Hàm điều chỉnh tốc độ quạt
void set_motor_speed(int gpio1, int gpio2, int state) {
        
    // Cấu hình các chân GPIO làm output
    esp_rom_gpio_pad_select_gpio(gpio1);
    esp_rom_gpio_pad_select_gpio(gpio2);
    gpio_set_direction(gpio1, GPIO_MODE_OUTPUT);
    gpio_set_direction(gpio2, GPIO_MODE_OUTPUT);
        
    // Cấu hình PWM
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = gpio1,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);
    switch(state) {
        case 0:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, DUTY_OFF);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            gpio_set_level(gpio2, 0);
            break;
        case 1:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, DUTY_LIGHT);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            gpio_set_level(gpio2, 1);
            break;
        case 2:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, DUTY_MEDIUM);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            gpio_set_level(gpio2, 1);
            break;
        case 3:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, DUTY_HIGH);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            gpio_set_level(gpio2, 1);
            break;
        }
        current_fan_state = state;
    }
    // Hàm lấy trạng thái hiện tại của quạt
int getStateFan() {
        return current_fan_state;
}
