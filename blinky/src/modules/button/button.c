#include "button.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);

static const struct gpio_dt_spec button5 = GPIO_DT_SPEC_GET_OR(BUTTON5_NODE, gpios, {0});
static struct gpio_callback button_cb_data;
static button_callback_t user_callback;

static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    static int64_t last_press = 0;
    int64_t now = k_uptime_get();

    if (now - last_press < DEBOUNCE_MS) return;
    last_press = now;

    if (user_callback) {
        user_callback(BUTTON_PRESSED);
    }
}

error_code_t button_init(void)
{
    if (!device_is_ready(button5.port)) {
        LOG_ERR("Button device not ready");
        return ERR_DEVICE_NOT_READY;
    }

    gpio_pin_configure_dt(&button5, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button5, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button5.pin));
    gpio_add_callback(button5.port, &button_cb_data);

    LOG_INF("Button initialized");
    return ERR_NONE;
}

error_code_t button_register_callback(button_callback_t callback)
{
    if (callback == NULL) {
        LOG_ERR("Invalid callback function");
        return ERR_INVALID_PARAM;
    }
    
    user_callback = callback;
    return ERR_NONE;
} 