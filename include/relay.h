/*
 * SPDX-FileCopyrightText: 2024 Fábio Souza
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file relay.h
 * @brief Relay control library for ESP32 using ESP-IDF framework.
 * 
 * This library provides an abstraction layer to control relays on the ESP32 platform.
 * It supports both Normally Open (NO) and Normally Closed (NC) relay configurations
 * and includes functions for basic and time-based relay operations.
 */

#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>
#include "esp_timer.h"
#include "esp_err.h"

// Forward declaration of the Relay structure
typedef struct Relay Relay;

// Function pointer type for relay actions
typedef void (*relay_action_t)(Relay *relay);

/** 
 * @brief Enumeration for relay types.
 * 
 * This enum specifies the type of relay configuration. A relay can be Normally Open (NO),
 * where the circuit is open when the relay is inactive, or Normally Closed (NC), where
 * the circuit is closed when the relay is inactive.
 */
typedef enum {
    RELAY_NO,
    RELAY_NC
} RelayType;




/**
 * @brief Structure to store relay configuration and state.
 * 
 * This structure holds essential information about the relay, including the GPIO pin,
 * relay type (NO or NC), current state (on or off), and a timer handle for time-based operations.
 */
struct Relay {
    int pin;
    bool is_on;
    RelayType type;
    esp_timer_handle_t timer;
    relay_action_t pending_action;
};

/**
 * @brief Initializes the relay with a specific GPIO pin, type, and initial state.
 * 
 * This function configures the relay by setting its pin as an output, establishing
 * the initial state, and creating a timer for delayed operations.
 * 
 * @param relay Pointer to the relay structure.
 * @param pin GPIO pin number connected to the relay.
 * @param type Relay type (Normally Open or Normally Closed).
 * @param initial_state Initial state of the relay (1 for on, 0 for off).
 * @return esp_err_t 
 *   - ESP_OK if initialization is successful.
 *   - ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_init(Relay *relay, int pin, RelayType type, bool initial_on);

/**
 * @brief Turns the relay on.
 * 
 * Sets the relay to the "on" state based on its configuration (NO or NC).
 * The function validates the GPIO pin before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t 
 *   - ESP_OK if successful.
 *   - ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_turn_on(Relay *relay);

/**
 * @brief Turns the relay off.
 * 
 * Sets the relay to the "off" state based on its configuration (NO or NC).
 * The function validates the GPIO pin before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t 
 *   - ESP_OK if successful.
 *   - ESP_ERR_INVALID_ARG if the pin is invalid.
 */
esp_err_t relay_turn_off(Relay *relay);

/*
*  @brief Toggles the current state of the relay.
 * 
 * This function changes the state of the relay from on to off, or from off to on,
 * depending on its current state. The function validates the GPIO pin before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @return esp_err_t 
 *   - ESP_OK if successful.
 *   - ESP_ERR_INVALID_ARG if the pin is invalid.
*/
esp_err_t relay_toggle(Relay *relay);

/**
 * @brief Gets the current state of the relay.
 * 
 * This function returns the current state of the relay, where `1` indicates it is on,
 * and `0` indicates it is off.
 * 
 * @param relay Pointer to the relay structure.
 * @return int Current state of the relay (1 for on, 0 for off).
 */
bool relay_get_status(Relay *relay);

/**
 * @brief Schedules the relay to turn on after a specified delay.
 * 
 * This function turns on the relay after a delay specified in milliseconds.
 * The delay must be a positive value. The function validates both the GPIO pin
 * and the delay value before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @param delay_ms Delay in milliseconds after which the relay will turn on.
 * @return esp_err_t 
 *   - ESP_OK if successful.
 *   - ESP_ERR_INVALID_ARG if the pin or delay is invalid.
 */
esp_err_t relay_turn_on_after(Relay *relay, int delay_ms);

/**
 * @brief Schedules the relay to turn off after a specified delay.
 * 
 * This function turns off the relay after a delay specified in milliseconds.
 * The delay must be a positive value. The function validates both the GPIO pin
 * and the delay value before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @param delay_ms Delay in milliseconds after which the relay will turn off.
 * @return esp_err_t 
 *   - ESP_OK if successful.
 *   - ESP_ERR_INVALID_ARG if the pin or delay is invalid.
 */
esp_err_t relay_turn_off_after(Relay *relay, int delay_ms);

/**
 * @brief Turns the relay on for a specific duration, then turns it off automatically.
 * 
 * The relay is activated immediately and stays on for the specified duration in milliseconds.
 * The duration must be a positive value. The function validates both the GPIO pin and
 * the duration value before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @param duration_ms Duration in milliseconds for which the relay will stay on.
 * @return esp_err_t 
 *   - ESP_OK if successful.
 *   - ESP_ERR_INVALID_ARG if the pin or duration is invalid.
 */
esp_err_t relay_pulse(Relay *relay, int duration_ms);

/**
 * @brief Turns the relay on immediately and schedules it to turn off after a specified duration.
 * 
 * This function turns on the relay immediately, then automatically turns it off after
 * the specified duration in milliseconds. The duration must be a positive value.
 * The function validates both the GPIO pin and the duration value before proceeding.
 * 
 * @param relay Pointer to the relay structure.
 * @param duration_ms Duration in milliseconds after which the relay will turn off.
 * @return esp_err_t 
 *   - ESP_OK if successful.
 *   - ESP_ERR_INVALID_ARG if the pin or duration is invalid.
 */
esp_err_t relay_turn_on_and_turn_off_after(Relay *relay, int duration_ms);

#endif
