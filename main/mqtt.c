#include "mqtt.h"
#include "DHT.h"
#include "led_lib.h"
#include "servo_lib.h"
#include "fan_lib.h"

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t global_client;
static mqtt_data_callback_t mqtt_data_callback = NULL;

uint32_t MQTT_CONNECTED = 0;

static void log_error_if_nonzero(const char *message, int error_code){
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNECTED = 1;
        if (getState() == 1 || getState() == 0) {
        //    msg_id = esp_mqtt_client_subscribe(client, "led_status/status", 0);
           msg_id = esp_mqtt_client_subscribe(client, "device01/light/light01/state", 0);
           ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        }
        if (getStateSer() == 1 || getStateSer() == 0) {
        //    msg_id = esp_mqtt_client_subscribe(client, "servo_status/status", 0);
           msg_id = esp_mqtt_client_subscribe(client, "device01/door/door01/state", 0);
           ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        }
        if (getStateFan() == 0 || getStateFan() == 1 || getStateFan() == 2 || getStateFan() == 3){
        //    msg_id = esp_mqtt_client_subscribe(client, "fan_status/status", 0);
           msg_id = esp_mqtt_client_subscribe(client, "device01/fan/fan01/state", 0);
           ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        }
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNECTED = 0;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //DHT data:
        int ret = readDHT();
        if (ret == DHT_OK) {
            float temperature = getTemperature();
            float humidity = getHumidity();
            ESP_LOGI(TAG, "Temperature: %.1f, Humidity: %.1f", temperature, humidity);
            char *json_str = convert_model_sensor_to_json(temperature, humidity);
            //ESP_LOGI(TAG,"%s",json_str);
            if (json_str != NULL) {
                msg_id = esp_mqtt_client_publish(client, "/sensor/data", json_str, 0, 0, 0);
                ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
                free(json_str);
            } else {
                ESP_LOGE(TAG, "Failed to create JSON string");
            }
        } else {
            errorHandler(ret);
    }

        //LED DATA:
        if (getState() == 1 || getState() == 0) {
            ESP_LOGI(TAG, "=== SIGNAL LED ===");
            int signal_led = getState();
            char *json_StateLed = convert_model_signalLight_to_json(signal_led);
            //ESP_LOGI(TAG, "%s", json_StateLed);
        if (json_StateLed != NULL) {
            msg_id = esp_mqtt_client_publish(client, "device01/light/light01/state", json_StateLed, 0, 0, 0);
            ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);
            free(json_StateLed);
        } else {
            ESP_LOGE(TAG, "Failed to create JSON string");
        }
    }
        //SERVO DATA:
        if (getStateSer() == 1 || getStateSer() == 0) {
           ESP_LOGI(TAG, "=== SIGNAL Servo ===");
           int signal_servo = getStateSer();
           char *json_StateServo = convert_model_signalDoor_to_json(signal_servo);
           //ESP_LOGI(TAG, "%s", json_StateServo);
        if (json_StateServo != NULL) {
            msg_id = esp_mqtt_client_publish(client, "device01/door/door01/state", json_StateServo, 0, 0, 0);
            ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);
            free(json_StateServo);
        } else {
            ESP_LOGE(TAG, "Failed to create JSON string");
        }
    }
        //FAN DATA:
        if (getStateFan() == 0 || getStateFan() == 1 || getStateFan() == 2 || getStateFan() == 3) {
           ESP_LOGI(TAG, "=== SIGNAL Fan ===");
           int signal_fan = getStateFan();
           char *json_StateFan = convert_model_signalFan_to_json(signal_fan);
           //ESP_LOGI(TAG, "%s", json_StateFan);
        if (json_StateFan != NULL) {
            msg_id = esp_mqtt_client_publish(client, "device01/fan/fan01/state", json_StateFan, 0, 0, 0);
            ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);
            free(json_StateFan);
        } else {
            ESP_LOGE(TAG, "Failed to create JSON string");
        }
    }
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if(mqtt_data_callback){
            event->data[event->data_len] = '\0';
            mqtt_data_callback(event->topic, event->data, event->data_len);// thực hiện hàm get_data_call_back
                                                    //mqtt_data_callback_: gán địa chỉ = get_data_call_back
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

//DHT convert:
char *convert_model_sensor_to_json(float temperature, float humidity){
    // create a new cJSON object
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        printf("Error: Failed to create cJSON object\n");
        return NULL;
    }
    // modify the JSON data
    char temp_str[10];
    char hum_str[10];
    snprintf(temp_str, sizeof(temp_str), "%.1f", temperature);
    snprintf(hum_str, sizeof(hum_str), "%.1f", humidity);

    cJSON_AddStringToObject(json, "temp", temp_str);
    cJSON_AddStringToObject(json, "hum", hum_str);

    // convert the cJSON object to a JSON string
    char *json_str = cJSON_PrintUnformatted(json);

    // free the JSON string and cJSON object
    cJSON_Delete(json);

    return json_str;
}

// 
char *convert_model_signalLight_to_json(int signal){
    // create a new cJSON object
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        printf("Error: Failed to create cJSON object");
        return NULL;
    }
    // modify the JSON data
    char state_str[10];
    if(signal == 1) snprintf(state_str, sizeof(state_str), "ON");
    else snprintf(state_str, sizeof(state_str), "OFF");
    cJSON *door01 = cJSON_CreateObject();
    if (door01 == NULL) {
        printf("Error: Failed to create cJSON door01 object");
        cJSON_Delete(json);
        return NULL;
    }

    cJSON_AddStringToObject(door01, "name", "Đèn");
    cJSON_AddStringToObject(door01, "state", state_str);
    cJSON_AddItemToObject(json, "light01", door01);
    // convert the cJSON object to a JSON string
    char *json_state = cJSON_PrintUnformatted(json);
    // free the JSON object (not the string)
    cJSON_Delete(json);

    return json_state;
}

//Door signal convert:
char *convert_model_signalDoor_to_json(int signal){
    // create a new cJSON object
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        printf("Error: Failed to create cJSON object");
        return NULL;
    }
    // modify the JSON data
    char state_str[10];
    if(signal == 1) snprintf(state_str, sizeof(state_str), "ON");
    else snprintf(state_str, sizeof(state_str), "OFF");
    cJSON *door01 = cJSON_CreateObject();
    if (door01 == NULL) {
        printf("Error: Failed to create cJSON door01 object");
        cJSON_Delete(json);
        return NULL;
    }

    cJSON_AddStringToObject(door01, "name", "Cửa");
    cJSON_AddStringToObject(door01, "state", state_str);
    cJSON_AddItemToObject(json, "door01", door01);
    // convert the cJSON object to a JSON string
    char *json_state = cJSON_PrintUnformatted(json);
    // free the JSON object (not the string)
    cJSON_Delete(json);

    return json_state;
}

//FAN signal convert:
char *convert_model_signalFan_to_json(int signal){
    // create a new cJSON object
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        printf("Error: Failed to create cJSON object");
        return NULL;
    }
    // modify the JSON data
    char state_str1[20];
    if(signal == 0) {
        snprintf(state_str1, sizeof(state_str1), "OFF");
    } else if(signal == 1){
        snprintf(state_str1, sizeof(state_str1), "LOW");
    }else if(signal == 2){
        snprintf(state_str1, sizeof(state_str1), "MEDIUM");
    }else if(signal == 3){
        snprintf(state_str1, sizeof(state_str1), "HIGH");
    } else {
        return -1;
    }
    cJSON *fan01 = cJSON_CreateObject();
    if (fan01 == NULL) {
        printf("Error: Failed to create cJSON fan01 object");
        cJSON_Delete(json);
        return NULL;
    }

    cJSON_AddStringToObject(fan01, "name", "Quạt");
    cJSON_AddStringToObject(fan01, "state", state_str1);
    cJSON_AddItemToObject(json, "fan01", fan01);
    // convert the cJSON object to a JSON string
    char *json_state = cJSON_PrintUnformatted(json);
    // free the JSON object (not the string)
    cJSON_Delete(json);

    return json_state;
}

//Return variable-global types esp_mqtt_client_handle_t
esp_mqtt_client_handle_t get_mqtt_client_handle(void){
    return global_client;
}

//Update and publish Json data
void mqtt_data_publish_update(){
    esp_mqtt_client_handle_t client = get_mqtt_client_handle();
    if (client != NULL)
    {
        char json_string[20]; // Đảm bảo bộ đệm đủ lớn
        sprintf(json_string, "{\"update\": true}"); // Sử dụng sprintf để định dạng
        // Gửi dữ liệu lên broker MQTT với chủ đề là "data"
        int msg_id = esp_mqtt_client_publish(client, "esp_pub", json_string, 0, 0, 0);
        if (msg_id < 0)
        {
            ESP_LOGE(TAG, "Failed to publish data to MQTT broker");
        }
        else
        {
            ESP_LOGI(TAG, "Published data to MQTT broker, msg_id=%d", msg_id);
        }
    }
    else
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
    }
}

// callback:
void mqtt_data_pt_set_callback(mqtt_data_callback_t mqtt_func_ptr){
    mqtt_data_callback = mqtt_func_ptr;
}

//MQTT received data callback
void mqtt_data_received_callback(const char *topic, const char *data, int data_len) {
    cJSON *json = cJSON_Parse(data);
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON data");
        return;
    }
    cJSON *device01 = cJSON_GetObjectItem(json, "device01");
    if(device01 != NULL){
       cJSON *fan = cJSON_GetObjectItem(device01, "fan");
       if (fan != NULL) {
        cJSON *fan01 = cJSON_GetObjectItem(fan, "fan01");
        if(fan01 != NULL){
           cJSON *state = cJSON_GetObjectItem(fan01, "state");
           if (state != NULL && cJSON_IsString(state)) {
                if (strcmp(state->valuestring, "OFF") == 0) {
                set_motor_speed(GPIO_NUM_13, GPIO_NUM_14, 0);
            } else if (strcmp(state->valuestring, "LOW") == 0) {
                set_motor_speed(GPIO_NUM_13, GPIO_NUM_14, 1);
            } else if (strcmp(state->valuestring, "MEDIUM") == 0) {
                set_motor_speed(GPIO_NUM_13, GPIO_NUM_14, 2);
            } else if (strcmp(state->valuestring, "HIGH") == 0) {
                set_motor_speed(GPIO_NUM_13, GPIO_NUM_14, 3);
            }
            }
          }
       }
    }
    
   // cJSON *device01 = cJSon_GetObjectItem(json, "device01");
    if(device01 != NULL){
       cJSON *light = cJSON_GetObjectItem(device01, "light");
       if (light != NULL) {
        cJSON *light01 = cJSON_GetObjectItem(light, "light01");
        if(light01 != NULL){
           cJSON *state = cJSON_GetObjectItem(light01, "state");
           if (state != NULL && cJSON_IsString(state)) {
               if (strcmp(state->valuestring, "ON") == 0) {
                // Turn on the LED
                led_set(1);
            } else if (strcmp(state->valuestring, "OFF") == 0) {
                // Turn off the LED
                led_set(0);
            }
            }
          }
       }
    }

    //cJSON *device01 = cJSon_GetObjectItem(json, "device01");
    if(device01 != NULL){
       cJSON *door = cJSON_GetObjectItem(device01, "door");
       if (door != NULL) {
       cJSON *door01 = cJSON_GetObjectItem(door, "door01");
          if (door01 != NULL) {
          cJSON *state = cJSON_GetObjectItem(door01, "state");
          if (state != NULL && cJSON_IsString(state)) {
           if (strcmp(state->valuestring, "ON") == 0) {
                // Turn on the servo
                servo_set_state(GPIO_NUM_23, 1);
               }else if (strcmp(state->valuestring, "OFF") == 0) {
                // Turn off the servo
                servo_set_state(GPIO_NUM_23, 0);
               }
            }
         }
       }
    }
    cJSON_Delete(json);
}

void mqtt_app_start(){
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = EXAMPLE_ESP_MQQT_BORKER_URI,
        .broker.address.port = EXAMPLE_ESP_MQQT_BORKER_PORT,
        .credentials.username = EXAMPLE_ESP_MQQT_CREDENTIALS_USERNAME,
        //.broker.address.transport = EXAMPLE_ESP_MQQT_BORKER_TRANSPORT,
        
    };

    global_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(global_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(global_client);

    // Đăng ký callback
    mqtt_data_pt_set_callback(mqtt_data_received_callback);
}