#include "ble.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ble, LOG_LEVEL_INF);

static ble_state_t current_state = BLE_STATE_DISCONNECTED;
static ble_event_callback_t event_callback;

error_code_t ble_init(void)
{
    // TODO: Implement actual BLE initialization
    LOG_INF("BLE initialization (stub)");
    return ERR_NONE;
}

error_code_t ble_start_advertising(void)
{
    if (current_state != BLE_STATE_DISCONNECTED) {
        LOG_ERR("Cannot start advertising in current state: %d", current_state);
        return ERR_INVALID_PARAM;
    }
    
    // TODO: Implement actual advertising
    current_state = BLE_STATE_ADVERTISING;
    LOG_INF("BLE advertising started");
    return ERR_NONE;
}

error_code_t ble_stop_advertising(void)
{
    if (current_state != BLE_STATE_ADVERTISING) {
        LOG_ERR("Cannot stop advertising in current state: %d", current_state);
        return ERR_INVALID_PARAM;
    }
    
    // TODO: Implement actual advertising stop
    current_state = BLE_STATE_DISCONNECTED;
    LOG_INF("BLE advertising stopped");
    return ERR_NONE;
}

error_code_t ble_register_callback(ble_event_callback_t callback)
{
    if (callback == NULL) {
        LOG_ERR("Invalid callback function");
        return ERR_INVALID_PARAM;
    }
    
    event_callback = callback;
    return ERR_NONE;
}

ble_state_t ble_get_state(void)
{
    return current_state;
} 