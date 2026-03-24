#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "relay.h"
#include "esp_log.h"

#define RELAY_PIN 21

static const char *TAG = "RelayExample";

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing relay example...");

    static Relay my_relay;

    ESP_ERROR_CHECK(relay_init(&my_relay, RELAY_PIN, RELAY_NO, false));

    // Example 1
    ESP_LOGI(TAG, "Turning relay on immediately...");
    ESP_ERROR_CHECK(relay_turn_on(&my_relay));
    ESP_LOGI(TAG, "State: %d", relay_get_status(&my_relay));
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Example 2
    ESP_LOGI(TAG, "Scheduling relay to turn off after 3 seconds...");
    ESP_ERROR_CHECK(relay_turn_off_after(&my_relay, 3000));
    vTaskDelay(pdMS_TO_TICKS(4000));
    ESP_LOGI(TAG, "State: %d", relay_get_status(&my_relay));

    // Example 3
    ESP_LOGI(TAG, "Pulsing relay for 5 seconds...");
    ESP_ERROR_CHECK(relay_pulse(&my_relay, 5000));
    vTaskDelay(pdMS_TO_TICKS(6000));
    ESP_LOGI(TAG, "State: %d", relay_get_status(&my_relay));

    // Example 4
    ESP_LOGI(TAG, "Turning relay on and scheduling off after 7 seconds...");
    ESP_ERROR_CHECK(relay_turn_on_and_turn_off_after(&my_relay, 7000));
    vTaskDelay(pdMS_TO_TICKS(8000));
    ESP_LOGI(TAG, "State: %d", relay_get_status(&my_relay));

    ESP_LOGI(TAG, "Relay example complete.");
}
