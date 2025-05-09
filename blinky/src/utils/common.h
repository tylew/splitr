#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

/* Version Information */
#define APP_VERSION_MAJOR    0
#define APP_VERSION_MINOR    1
#define APP_VERSION_PATCH    0

/* Common Timing Definitions */
#define DEBOUNCE_MS         50
#define LED_FADE_INTERVAL_MS 20
#define BLE_CONN_TIMEOUT_MS 5000

/* Device Tree Aliases */
#define BUTTON5_NODE    DT_ALIAS(sw4)
#define LED_RED_NODE    DT_ALIAS(led0)
#define LED_GREEN_NODE  DT_ALIAS(led1)
#define LED_BLUE_NODE   DT_ALIAS(led2)

/* Error Codes */
typedef enum {
    ERR_NONE = 0,
    ERR_DEVICE_NOT_READY = -1,
    ERR_INVALID_PARAM = -2,
    ERR_BLE_INIT_FAILED = -3,
    ERR_GPIO_FAILED = -4
} error_code_t;

/* LED Color Definitions */
typedef enum {
    LED_COLOR_RED = 0,
    LED_COLOR_GREEN = 1,
    LED_COLOR_BLUE = 2,
    LED_COLOR_MAX
} led_color_t;

/* Function Status Check Macro */
#define CHECK_ERROR(x) \
    do { \
        error_code_t err = (x); \
        if (err != ERR_NONE) { \
            LOG_ERR("Error %d at %s:%d", err, __FILE__, __LINE__); \
            return err; \
        } \
    } while(0)

#endif /* UTILS_COMMON_H */ 