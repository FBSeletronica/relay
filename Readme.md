# Relay

This component is designed to provide an abstraction layer for controlling relays on ESP32-based systems using ESP-IDF. It includes flexible functions to manage Normally Open (NO) and Normally Closed (NC) relays, time-based control, and automatic scheduling of relay operations.

## Features

- **Relay Types**: Supports Normally Open (NO) and Normally Closed (NC) configurations.
- **Timed Control**: Allows for precise relay control, including scheduled on/off operations.
- **Automatic Shutdown**: Functions to activate the relay and automatically turn it off after a specified duration.

## Installation

To install this component, use the ESP-IDF Component Manager. Add the following to your `idf_component.yml` in your main project folder:

```yaml
dependencies:
  relay: "1.0.0"  # Replace with the desired version
```

Then, in your project directory, run:

```bash
idf.py add-dependency
```

This command will download and add the component to your ESP-IDF project.

## Usage

After including the component in your project, include the header file `relay.h` in your source files. Initialize and control your relay by following the steps below.

### Initialization

To initialize the relay, use `relay_init`. Specify the GPIO pin, relay type (NO or NC), and initial state (on or off).

```c
#include "relay.h"

Relay my_relay;

void app_main(void) {
    // Initialize the relay as Normally Open (NO), starting in the "off" state
    relay_init(&my_relay, RELAY_PIN, RELAY_NO, 0);
}
```

### Basic Control

#### Turning the Relay On and Off

Use `relay_turn_on` and `relay_turn_off` to manually control the relay.

```c
// Turn the relay on
relay_turn_on(&my_relay);

// Turn the relay off
relay_turn_off(&my_relay);
```

#### Checking Relay Status

To get the current state of the relay, use `relay_get_status`. This function returns `1` if the relay is on and `0` if it is off.

```c
int status = relay_get_status(&my_relay);
```

### Timed Control

#### Delayed Turn On and Off

The functions `relay_turn_on_after` and `relay_turn_off_after` allow for delayed activation or deactivation of the relay.

```c
// Turn on the relay after 3 seconds
relay_turn_on_after(&my_relay, 3000); // Delay in milliseconds

// Turn off the relay after 5 seconds
relay_turn_off_after(&my_relay, 5000); // Delay in milliseconds
```

#### Pulse Control

The `relay_pulse` function turns on the relay for a specified duration, then automatically turns it off.

```c
// Turn on the relay for 5 seconds, then turn it off automatically
relay_pulse(&my_relay, 5000); // Duration in milliseconds
```

#### Turn On and Turn Off Automatically After a Duration

To immediately turn on the relay and automatically turn it off after a set duration, use `relay_turn_on_and_turn_off_after`.

```c
// Immediately turn on the relay and turn it off after 7 seconds
relay_turn_on_and_turn_off_after(&my_relay, 7000); // Duration in milliseconds
```

### Full Example

Here's a full example of using the relay component in an ESP-IDF project.

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "relay.h"
#include "esp_log.h"

#define RELAY_PIN 18 // GPIO pin connected to the relay

static const char *TAG = "RelayExample";

void app_main(void) {
    ESP_LOGI(TAG, "Initializing relay example...");

    // Initialize the relay as Normally Open, initially off
    Relay my_relay;
    relay_init(&my_relay, RELAY_PIN, RELAY_NO, 0);

    // Example 1: Turn the relay on immediately
    ESP_LOGI(TAG, "Turning relay on immediately...");
    relay_turn_on(&my_relay);
    vTaskDelay(pdMS_TO_TICKS(2000)); // Wait 2 seconds

    // Example 2: Schedule the relay to turn off after 3 seconds
    ESP_LOGI(TAG, "Scheduling relay to turn off after 3 seconds...");
    relay_turn_off_after(&my_relay, 3000);
    vTaskDelay(pdMS_TO_TICKS(4000)); // Wait 4 seconds to observe

    // Example 3: Pulse the relay for 5 seconds
    ESP_LOGI(TAG, "Pulsing relay for 5 seconds...");
    relay_pulse(&my_relay, 5000);
    vTaskDelay(pdMS_TO_TICKS(6000)); // Wait 6 seconds to observe

    // Example 4: Turn the relay on and automatically turn off after 7 seconds
    ESP_LOGI(TAG, "Turning relay on and scheduling it to turn off after 7 seconds...");
    relay_turn_on_and_turn_off_after(&my_relay, 7000);
    vTaskDelay(pdMS_TO_TICKS(8000)); // Wait 8 seconds to observe

    ESP_LOGI(TAG, "Relay example complete.");
}
```

## License

This component is licensed under the Apache-2.0 license. See [license.txt](license.txt) for details.


