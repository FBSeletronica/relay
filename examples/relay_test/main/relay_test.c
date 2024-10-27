#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "relay.h"
#include "esp_log.h"

#define RELAY_PIN 2 // GPIO pin connected to the relay

static const char *TAG = "RelayExample";

void app_main(void) {
    ESP_LOGI(TAG, "Initializing relay example...");

    // Declare and initialize the relay
    Relay my_relay;
    relay_init(&my_relay, RELAY_PIN, RELAY_NO, 0); // Normally Open (NO) relay, initially off
    
    // Example 1: Turn relay on immediately
    ESP_LOGI(TAG, "Turning relay on immediately...");
    relay_turn_on(&my_relay);
    vTaskDelay(pdMS_TO_TICKS(2000)); // Wait 2 seconds

    // Example 2: Turn relay off after 3 seconds
    ESP_LOGI(TAG, "Scheduling relay to turn off after 3 seconds...");
    relay_turn_off_after(&my_relay, 3000); // Turns off after 3 seconds
    vTaskDelay(pdMS_TO_TICKS(4000)); // Wait 4 seconds to observe

    // Example 3: Pulse relay for 5 seconds
    ESP_LOGI(TAG, "Pulsing relay for 5 seconds...");
    relay_pulse(&my_relay, 5000); // Turn on and off after 5 seconds
    vTaskDelay(pdMS_TO_TICKS(6000)); // Wait 6 seconds to observe

    // Example 4: Turn relay on and automatically turn off after 7 seconds
    ESP_LOGI(TAG, "Turning relay on and scheduling it to turn off after 7 seconds...");
    relay_turn_on_and_turn_off_after(&my_relay, 7000); // Turn on and auto off after 7 seconds
    vTaskDelay(pdMS_TO_TICKS(8000)); // Wait 8 seconds to observe

    // Finalize example
    ESP_LOGI(TAG, "Relay example complete.");
}
