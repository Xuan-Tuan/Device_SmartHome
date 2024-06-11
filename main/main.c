#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_system.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "DHT.h"
#include "wifi.h"    // Thêm thư viện WiFi
#include "mqtt.h"    // Thêm thư viện MQTT
#include "led_lib.h"
#include "servo_lib.h"
#include "fan_lib.h"

static const char *TAG = "DHT";

//DHT:
void DHT_task(void *pvParameter)
{
    setDHTgpio(GPIO_NUM_27);
    ESP_LOGI(TAG, "Starting DHT Task\n\n");

    while (1)
    {
        ESP_LOGI(TAG, "=== Reading DHT ===\n");
        int ret = readDHT();

        errorHandler(ret);

        float humidity = getHumidity();
        float temperature = getTemperature();

        ESP_LOGI(TAG, "Hum: %.1f Tmp: %.1f\n", humidity, temperature);
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}

//LED:
void LED_task(void *pvParameter)
{
    setLEDgpio(GPIO_NUM_21);
    int led_st = 0;
    // Biến trạng thái của LED
    while (1)
    {
        // Bật hoặc tắt LED tùy theo trạng thái hiện tại
        led_set(led_st);
        getState();
        // Chuyển đổi trạng thái để xen kẽ giữa bật và tắt
        led_st = !led_st;
        // Chờ 5 giây
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

//Servo
// void Servo_task(void *pvParameter)
// {
//     int servo_st;
//     setServogpio(GPIO_NUM_23);
//     servo_set_state(GPIO_NUM_23,servo_st);
// }

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_log_level_set("*", ESP_LOG_INFO);

    // Initialize WiFi
    ESP_LOGI(TAG, "Initializing WiFi...");
    wifi_init_sta();

    esp_rom_gpio_pad_select_gpio(GPIO_NUM_27);

    // Create the DHT task
    // xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);
    // //Create LED task:
    // xTaskCreate(&LED_task, "LED_task", 2048, NULL, 4, NULL);
    // //Create Servo task:
    // xTaskCreate(&Servo_task, "Servo_task", 2048, NULL, 3, NULL);

    // Initialize MQTT
    ESP_LOGI(TAG, "Initializing MQTT...");
    mqtt_app_start();

}
