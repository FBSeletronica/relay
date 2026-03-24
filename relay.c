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

// Helper function to apply the relay state based on its type (NO or NC)
static void relay_apply(Relay *relay, bool on)
{
    int level;

    if (relay->type == RELAY_NO) {
        level = on ? 1 : 0;
    } else {
        level = on ? 0 : 1;
    }

    gpio_set_level(relay->pin, level);
    relay->is_on = on;
}

/* Timer callback */
static void relay_timer_callback(void *arg)
{
    Relay *relay = (Relay *)arg;

    if (relay->pending_action) {
        relay->pending_action(relay);
    }
}


/* Actions */
static void action_turn_on(Relay *relay)
{
    relay_apply(relay, true);
}

static void action_turn_off(Relay *relay)
{
    relay_apply(relay, false);
}




/**
* @brief Initializes the relay with a specific GPIO pin, type, and initial state.
 * 
 * This function configures the relay by setting its pin as an output, establishing
 * the initial state, and creating a timer for delayed operations.
 * 
 * @param relay Pointer to the relay structure.
 * @param pin GPIO pin number connected to the relay.
 * @param type Type of relay (RELAY_NO or RELAY_NC).
 * @param initial_on Initial state of the relay (true for on, false for off).
 * @return esp_err_t ESP_OK if initialized successfully, ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_init(Relay *relay, int pin, RelayType type, bool initial_on)
{
    if (!GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        ESP_LOGE(TAG, "Invalid GPIO: %d", pin);
        return ESP_ERR_INVALID_ARG;
    }

    relay->pin = pin;
    relay->type = type;
    relay->pending_action = NULL;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
    };

    gpio_config(&io_conf);

    relay_apply(relay, initial_on);

    esp_timer_create_args_t timer_args = {
        .callback = relay_timer_callback,
        .arg = relay,
        .name = "relay_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &relay->timer));

    return ESP_OK;
}

/*
* @brief Turns the relay on.
 * 
 * Sets the relay to the "on" state based on its configuration (NO or NC).
 * The function validates the GPIO pin before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin is invalid.   
*/
esp_err_t relay_turn_on(Relay *relay)
{
    relay_apply(relay, true);
    return ESP_OK;
}

/*
* @brief Turns the relay off.
 * 
 * Sets the relay to the "off" state based on its configuration (NO or NC).
 * The function validates the GPIO pin before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_turn_off(Relay *relay)
{
    relay_apply(relay, false);
    return ESP_OK;
}

/*
* @brief Toggles the current state of the relay.
 * 
 * This function changes the state of the relay from on to off, or from off to on,
 * depending on its current state. The function validates the GPIO pin before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t ESP_OK if successful, ESP_ERR_INVALID_ARG if the pin is invalid.
*/
esp_err_t relay_toggle(Relay *relay)
{
    relay_apply(relay, !relay->is_on);
    return ESP_OK;
}

/*
* @brief Gets the current state of the relay.
 * 
 * This function returns the current state of the relay, where `true` indicates it is on,
 * and `false` indicates it is off.
 * 
 * @param relay Pointer to the relay structure.
 * @return bool Current state of the relay (true for on, false for off).
*/
bool relay_get_status(Relay *relay)
{
    return relay->is_on;
}


/*
* @brief Helper function to start a timer for delayed relay actions.
 * 
 * This function sets up a timer to execute a specified action (turn on or turn off)
 * after a given delay in milliseconds. It validates the delay and manages the timer state.
 * 
 * @param relay Pointer to the relay structure.
 * @param delay_ms Delay in milliseconds before the action is executed.
 * @param action Function pointer to the action to be performed when the timer expires.
 * @return esp_err_t ESP_OK if the timer was started successfully, ESP_ERR_INVALID_ARG if the delay is invalid.
*/
static esp_err_t relay_start_timer(Relay *relay, int delay_ms, relay_action_t action)
{
    if (delay_ms <= 0) {
        ESP_LOGE(TAG, "Invalid delay: %d", delay_ms);
        return ESP_ERR_INVALID_ARG;
    }

    esp_timer_stop(relay->timer); // safe mesmo se não estiver rodando

    relay->pending_action = action;

    return esp_timer_start_once(relay->timer, delay_ms * 1000);
}

/*
* @brief Turns the relay on after a specified delay.
 * 
 * This function schedules the relay to turn on after a given delay in milliseconds.
 * It validates the delay and manages the timer state.
 * 
 * @param relay Pointer to the relay structure.
 * @param delay_ms Delay in milliseconds before the relay turns on.
 * @return esp_err_t ESP_OK if the timer was started successfully, ESP_ERR_INVALID_ARG if the delay is invalid.
 */
esp_err_t relay_turn_on_after(Relay *relay, int delay_ms)
{
    return relay_start_timer(relay, delay_ms, action_turn_on);
}

/*
* @brief Turns the relay off after a specified delay.
 * 
 * This function schedules the relay to turn off after a given delay in milliseconds.
 * It validates the delay and manages the timer state.
 * 
 * @param relay Pointer to the relay structure.
 * @param delay_ms Delay in milliseconds before the relay turns off.
 * @return esp_err_t ESP_OK if the timer was started successfully, ESP_ERR_INVALID_ARG if the delay is invalid.
 */
esp_err_t relay_turn_off_after(Relay *relay, int delay_ms)
{
    return relay_start_timer(relay, delay_ms, action_turn_off);
}

/*
* @brief Triggers a pulse on the relay for a specified duration.
 * 
 * This function turns the relay on and then off after a given duration in milliseconds.
 * It validates the duration and manages the timer state.
 * 
 * @param relay Pointer to the relay structure.
 * @param duration_ms Duration in milliseconds for which the relay remains on.
 * @return esp_err_t ESP_OK if the pulse was triggered successfully, ESP_ERR_INVALID_ARG if the duration is invalid.
 */
esp_err_t relay_pulse(Relay *relay, int duration_ms)
{
    if (duration_ms <= 0) {
        ESP_LOGE(TAG, "Invalid duration: %d", duration_ms);
        return ESP_ERR_INVALID_ARG;
    }

    relay_apply(relay, true);

    return relay_start_timer(relay, duration_ms, action_turn_off);
}

/*
* @brief Turns the relay on and then off after a specified duration.
 * 
 * This function schedules the relay to turn on and then off after a given duration in milliseconds.
 * It validates the duration and manages the timer state.
 * 
 * @param relay Pointer to the relay structure.
 * @param duration_ms Duration in milliseconds for which the relay remains on.
 * @return esp_err_t ESP_OK if the action was scheduled successfully, ESP_ERR_INVALID_ARG if the duration is invalid.
 */
esp_err_t relay_turn_on_and_turn_off_after(Relay *relay, int duration_ms)
{
    return relay_pulse(relay, duration_ms);
}