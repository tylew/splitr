#ifndef MODULES_BLE_H
#define MODULES_BLE_H

#include "../../utils/common.h"

/* BLE connection states */
typedef enum {
    BLE_STATE_DISCONNECTED = 0,
    BLE_STATE_ADVERTISING,
    BLE_STATE_CONNECTED
} ble_state_t;

/* BLE event types */
typedef enum {
    BLE_EVT_CONNECTED = 0,
    BLE_EVT_DISCONNECTED,
    BLE_EVT_COLOR_CHANGED,
    BLE_EVT_ERROR
} ble_event_t;

/* BLE event callback type */
typedef void (*ble_event_callback_t)(ble_event_t event, void *data);

/* Initialize BLE functionality */
error_code_t ble_init(void);

/* Start/Stop BLE advertising */
error_code_t ble_start_advertising(void);
error_code_t ble_stop_advertising(void);

/* Register for BLE events */
error_code_t ble_register_callback(ble_event_callback_t callback);

/* Get current BLE state */
ble_state_t ble_get_state(void);

#endif /* MODULES_BLE_H */ 