// #ifndef _MQTT_H
// #define _MQTT_H
// #include <stdint.h>
// #include <stdio.h>
// #include <stdint.h>
// #include <stddef.h>
// #include <string.h>
// #include "esp_wifi.h"
// #include "esp_system.h"
// #include "esp_event.h"
// #include "esp_netif.h"

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/semphr.h"
// #include "freertos/queue.h"

// #include "lwip/sockets.h"
// #include "lwip/dns.h"
// #include "lwip/netdb.h"

// #include "esp_log.h"
// #include "mqtt_client.h"
// #include <cJSON.h>

// #define EXAMPLE_ESP_MQQT_BORKER_URI "mqtt://mqtt.flespi.io"
// #define EXAMPLE_ESP_MQQT_BORKER_PORT 1883
// #define EXAMPLE_ESP_MQQT_BORKER_TRANSPORT MQTT_TRANSPORT_OVER_TCP
// #define EXAMPLE_ESP_MQQT_CREDENTIALS_USERNAME "V0ZvuCxcbFBGUcQTfdIIstftyS5uMWVSzke2BNcwFfjKfqPaAIl1bQUhnU7ngybk"
// esp_mqtt_client_handle_t get_mqtt_client_handle(void);
// extern uint32_t MQTT_CONNECTED;

// typedef void (*mqtt_data_callback_t)(const char *topic, const char *data, uint16_t length);

// void mqtt_data_publish_update();
// void mqtt_data_received_callback(const char *topic, const char *data, int data_len);
// void mqtt_data_pt_set_callback(mqtt_data_callback_t mqtt_func_ptr);

// //convert
// char *convert_model_sensor_to_json(float temperature, float humidity);
// char *convert_model_signaldiv_to_json(int signal);
// char *convert_model_signalFan_to_json(int signal);

// void mqtt_app_start(void);

// #endif
#ifndef _MQTT_H
#define _MQTT_H
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include <cJSON.h>

#define EXAMPLE_ESP_MQQT_BORKER_URI "mqtt://mqtt.flespi.io"
#define EXAMPLE_ESP_MQQT_BORKER_PORT 1883
#define EXAMPLE_ESP_MQQT_BORKER_TRANSPORT MQTT_TRANSPORT_OVER_TCP
#define EXAMPLE_ESP_MQQT_CREDENTIALS_USERNAME "UdEQHxCaURMk4Rtvo6u46Dcq6xG3pdfT8jBj0OILyH1o5FsbegWbDN8fpHcV3g40"
esp_mqtt_client_handle_t get_mqtt_client_handle(void);
extern uint32_t MQTT_CONNECTED;

typedef void (*mqtt_data_callback_t)(const char *topic, const char *data, uint16_t length);

void mqtt_data_publish_update();
void mqtt_data_received_callback(const char *topic, const char *data, int data_len);
void mqtt_data_pt_set_callback(mqtt_data_callback_t mqtt_func_ptr);

//convert
char *convert_model_sensor_to_json(float temperature, float humidity);
char *convert_model_signalDoor_to_json(int signal);
char *convert_model_signalLight_to_json(int signal);
char *convert_model_signalFan_to_json(int signal);

void mqtt_app_start(void);

#endif