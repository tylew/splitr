#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "button/button.h"
#include "led/led.h"
#include "ble/ble.h"

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);

static led_color_t current_color = LED_COLOR_RED;

static void button_pressed_callback(button_event_t event)
{
    if (event == BUTTON_PRESSED) {
        current_color = (current_color + 1) % LED_COLOR_MAX;
        led_set_color(current_color, LED_FULL);
        LOG_INF("Color changed to %d", current_color);
    }
}

static void ble_event_callback(ble_event_t event, void *data)
{
    switch (event) {
        case BLE_EVT_CONNECTED:
            LOG_INF("BLE connected");
            break;
        case BLE_EVT_DISCONNECTED:
            LOG_INF("BLE disconnected");
            break;
        case BLE_EVT_COLOR_CHANGED:
            if (data != NULL) {
                led_color_t color = *(led_color_t *)data;
                led_set_color(color, LED_FULL);
                current_color = color;
                LOG_INF("Color changed via BLE to %d", color);
            }
            break;
        case BLE_EVT_ERROR:
            LOG_ERR("BLE error occurred");
            break;
    }
}

void main(void)
{
    LOG_INF("App start: setting up");

    CHECK_ERROR(button_init());
    CHECK_ERROR(led_init());
    CHECK_ERROR(ble_init());

    CHECK_ERROR(button_register_callback(button_pressed_callback));
    CHECK_ERROR(ble_register_callback(ble_event_callback));
    
    led_set_color(current_color, LED_FULL);
    ble_start_advertising();

    LOG_INF("Application initialized successfully");

    while (1) {
        k_sleep(K_FOREVER);
    }
}
