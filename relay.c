/*
 * SPDX-FileCopyrightText: 2024 Fábio Souza
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file relay.c
 * @brief Relay control implementation for ESP32 using ESP-IDF framework.
 */

#include <stdio.h>
#include "relay.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "Relay";

/**
 * @brief Callback function to automatically turn off the relay after a delay.
 * 
 * This function is called by the timer to turn off the relay automatically
 * when a timed operation is completed.
 * 
 * @param arg Pointer to the relay structure.
 */
static void relay_auto_turn_off_callback(void *arg) {
    Relay *relay = (Relay *)arg;
    relay_turn_off(relay); // Turn off the relay
}

/**
 * @brief Initializes the relay with a specified GPIO pin, type, and initial state.
 * 
 * This function configures the relay by setting its pin as an output, configuring 
 * the initial state, and creating a timer for time-based control.
 * 
 * @param relay Pointer to the relay structure.
 * @param pin GPIO pin number connected to the relay.
 * @param type Relay type (Normally Open or Normally Closed).
 * @param initial_state Initial state of the relay (1 for on, 0 for off).
 * @return esp_err_t ESP_OK if initialized successfully, ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_init(Relay *relay, int pin, RelayType type, int initial_state) {
    if (!GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        ESP_LOGE(TAG, "Invalid GPIO pin: %d", pin);
        return ESP_ERR_INVALID_ARG;
    }
    relay->pin = pin;
    relay->type = type;
    relay->state = (type == RELAY_NO) ? initial_state : !initial_state;

    gpio_set_direction(relay->pin, GPIO_MODE_OUTPUT);
    gpio_set_level(relay->pin, relay->state);

    // Set up the timer for time-based control
    esp_timer_create_args_t timer_args = {
        .callback = &relay_auto_turn_off_callback,
        .arg = relay,
        .name = "relay_auto_off_timer"
    };
    esp_timer_create(&timer_args, &relay->timer);

    return ESP_OK;
}

/**
 * @brief Turns the relay on.
 * 
 * Sets the relay to the "on" state. This function first checks if the configured
 * GPIO pin is valid.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_turn_on(Relay *relay) {
    if (!GPIO_IS_VALID_OUTPUT_GPIO(relay->pin)) {
        ESP_LOGE(TAG, "Invalid GPIO pin: %d", relay->pin);
        return ESP_ERR_INVALID_ARG;
    }
    relay->state = (relay->type == RELAY_NO) ? 1 : 0;
    gpio_set_level(relay->pin, relay->state);
    return ESP_OK;
}

/**
 * @brief Turns the relay off.
 * 
 * Sets the relay to the "off" state. This function first checks if the configured
 * GPIO pin is valid.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_turn_off(Relay *relay) {
    if (!GPIO_IS_VALID_OUTPUT_GPIO(relay->pin)) {
        ESP_LOGE(TAG, "Invalid GPIO pin: %d", relay->pin);
        return ESP_ERR_INVALID_ARG;
    }
    relay->state = (relay->type == RELAY_NO) ? 0 : 1;
    gpio_set_level(relay->pin, relay->state);
    return ESP_OK;
}

/**
 * @brief Gets the current state of the relay.
 * 
 * This function returns the current state of the relay, where `1` indicates on 
 * and `0` indicates off.
 * 
 * @param relay Pointer to the relay structure.
 * @return int Current state of the relay (1 for on, 0 for off).
 */
int relay_get_status(Relay *relay) {
    return relay->state;
}

/**
 * @brief Schedules the relay to turn on after a specified delay.
 * 
 * This function turns on the relay after a delay in milliseconds. The delay must be positive.
 * 
 * @param relay Pointer to the relay structure.
 * @param delay_ms Delay in milliseconds after which the relay will turn on.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin or delay is invalid.
 */
esp_err_t relay_turn_on_after(Relay *relay, int delay_ms) {
    if (!GPIO_IS_VALID_OUTPUT_GPIO(relay->pin)) {
        ESP_LOGE(TAG, "Invalid GPIO pin: %d", relay->pin);
        return ESP_ERR_INVALID_ARG;
    }
    if (delay_ms <= 0) {
        ESP_LOGE(TAG, "Invalid delay: %d ms. Delay must be positive.", delay_ms);
        return ESP_ERR_INVALID_ARG;
    }
    esp_timer_stop(relay->timer);
    relay->state = 0;
    esp_timer_start_once(relay->timer, delay_ms * 1000);
    return ESP_OK;
}

/**
 * @brief Schedules the relay to turn off after a specified delay.
 * 
 * This function turns off the relay after a delay in milliseconds. The delay must be positive.
 * 
 * @param relay Pointer to the relay structure.
 * @param delay_ms Delay in milliseconds after which the relay will turn off.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin or delay is invalid.
 */
esp_err_t relay_turn_off_after(Relay *relay, int delay_ms) {
    if (!GPIO_IS_VALID_OUTPUT_GPIO(relay->pin)) {
        ESP_LOGE(TAG, "Invalid GPIO pin: %d", relay->pin);
        return ESP_ERR_INVALID_ARG;
    }
    if (delay_ms <= 0) {
        ESP_LOGE(TAG, "Invalid delay: %d ms. Delay must be positive.", delay_ms);
        return ESP_ERR_INVALID_ARG;
    }
    esp_timer_stop(relay->timer);
    relay->state = 1;
    esp_timer_start_once(relay->timer, delay_ms * 1000);
    return ESP_OK;
}

/**
 * @brief Turns the relay on for a specified duration, then turns it off automatically.
 * 
 * The relay is activated immediately and will be turned off after the specified duration in milliseconds.
 * The duration must be positive.
 * 
 * @param relay Pointer to the relay structure.
 * @param duration_ms Duration in milliseconds for which the relay will stay on.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin or duration is invalid.
 */
esp_err_t relay_pulse(Relay *relay, int duration_ms) {
    if (!GPIO_IS_VALID_OUTPUT_GPIO(relay->pin)) {
        ESP_LOGE(TAG, "Invalid GPIO pin: %d", relay->pin);
        return ESP_ERR_INVALID_ARG;
    }
    if (duration_ms <= 0) {
        ESP_LOGE(TAG, "Invalid duration: %d ms. Duration must be positive.", duration_ms);
        return ESP_ERR_INVALID_ARG;
    }
    relay_turn_on(relay);
    esp_timer_stop(relay->timer);
    esp_timer_start_once(relay->timer, duration_ms * 1000);
    return ESP_OK;
}

/**
 * @brief Turns the relay on immediately and schedules it to turn off after a specified duration.
 * 
 * This function turns on the relay immediately, then automatically turns it off after
 * the specified duration in milliseconds. The duration must be positive.
 * 
 * @param relay Pointer to the relay structure.
 * @param duration_ms Duration in milliseconds after which the relay will turn off.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin or duration is invalid.
 */
esp_err_t relay_turn_on_and_turn_off_after(Relay *relay, int duration_ms) {
    if (!GPIO_IS_VALID_OUTPUT_GPIO(relay->pin)) {
        ESP_LOGE(TAG, "Invalid GPIO pin: %d", relay->pin);
        return ESP_ERR_INVALID_ARG;
    }
    if (duration_ms <= 0) {
        ESP_LOGE(TAG, "Invalid duration: %d ms. Duration must be positive.", duration_ms);
        return ESP_ERR_INVALID_ARG;
    }
    relay_turn_on(relay);
    esp_timer_stop(relay->timer);
    esp_timer_start_once(relay->timer, duration_ms * 1000);
    return ESP_OK;
}
