# Relay

This component provides a robust and flexible abstraction layer for controlling relays on ESP32-based systems using ESP-IDF.

It supports both Normally Open (NO) and Normally Closed (NC) relays, along with advanced time-based control using high-resolution timers (`esp_timer`).

---

## ⚠️ Breaking Changes (v2.0.0)

* `Relay.state` replaced by `is_on` (logical state)
* `relay_init` now uses `bool initial_on` instead of `int`
* Timer behavior redesigned using internal action callbacks
* Delayed functions (`*_after`) now behave correctly

> This version is **NOT backward compatible** with v1.x

---

## Features

* **Relay Types**: Supports Normally Open (NO) and Normally Closed (NC) configurations
* **Timed Control**: Schedule on/off operations with millisecond precision
* **Automatic Shutdown**: Activate the relay and turn it off automatically after a duration
* **Safe Timer Handling**: Prevents inconsistent states in delayed operations
* **Logical State Abstraction**: Separates logical state from GPIO electrical level
* **Extensible Design**: Architecture ready for future features (queues, schedulers, etc.)

---

## Installation

Using ESP-IDF Component Manager:

```bash
idf.py add-dependency "relay^2.0.0"
```

Or manually in your `idf_component.yml`:

```yaml
dependencies:
  relay: "^2.0.0"
```

---

## Usage

Include the header in your source file:

```c
#include "relay.h"
```

---

## Initialization

```c
Relay my_relay;

void app_main(void)
{
    relay_init(&my_relay, RELAY_PIN, RELAY_NO, false);
}
```

---

## Basic Control

### Turn On / Off

```c
relay_turn_on(&my_relay);
relay_turn_off(&my_relay);
```

---

### Toggle

```c
relay_toggle(&my_relay);
```

---

### Get Status

```c
bool status = relay_get_status(&my_relay);
```

---

## Timed Control

### Turn On / Off After Delay

```c
relay_turn_on_after(&my_relay, 3000);  // Turn ON after 3 seconds
relay_turn_off_after(&my_relay, 5000); // Turn OFF after 5 seconds
```

---

### Pulse

```c
relay_pulse(&my_relay, 5000); // ON for 5 seconds, then OFF
```

---

### Turn On and Auto-Off

```c
relay_turn_on_and_turn_off_after(&my_relay, 7000);
```

---

## ⚠️ Notes

* Only one timer operation can be active per relay
* Calling another timed function will override the previous scheduled action
* The component uses `esp_timer`, so callbacks run in timer context

---

## Full Example

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "relay.h"
#include "esp_log.h"

#define RELAY_PIN 18

static const char *TAG = "RelayExample";

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing relay example...");

    static Relay my_relay;

    ESP_ERROR_CHECK(relay_init(&my_relay, RELAY_PIN, RELAY_NO, false));

    // Example 1: Turn ON
    ESP_LOGI(TAG, "Turning relay ON...");
    ESP_ERROR_CHECK(relay_turn_on(&my_relay));
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Example 2: Turn OFF after delay
    ESP_LOGI(TAG, "Scheduling OFF in 3 seconds...");
    ESP_ERROR_CHECK(relay_turn_off_after(&my_relay, 3000));
    vTaskDelay(pdMS_TO_TICKS(4000));

    // Example 3: Pulse
    ESP_LOGI(TAG, "Pulsing relay for 5 seconds...");
    ESP_ERROR_CHECK(relay_pulse(&my_relay, 5000));
    vTaskDelay(pdMS_TO_TICKS(6000));

    // Example 4: ON and auto OFF
    ESP_LOGI(TAG, "Turning ON and auto OFF in 7 seconds...");
    ESP_ERROR_CHECK(relay_turn_on_and_turn_off_after(&my_relay, 7000));
    vTaskDelay(pdMS_TO_TICKS(8000));

    // Status check
    bool status = relay_get_status(&my_relay);
    ESP_LOGI(TAG, "Final state: %d", status);

    ESP_LOGI(TAG, "Relay example complete.");
}
```

---

## Why use this component?

* Simplifies relay control logic
* Avoids common mistakes with NO/NC relays
* Reliable and predictable timer-based operations
* Clean architecture suitable for production systems

---

## License

This component is licensed under the Apache-2.0 license. See `LICENSE` for details.
