#include "led.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led, LOG_LEVEL_INF);

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

static led_color_t current_color = LED_COLOR_RED;
static led_brightness_t current_brightness = LED_FULL;

error_code_t led_set_color(led_color_t color, led_brightness_t brightness)
{
    if (color >= LED_COLOR_MAX) {
        LOG_ERR("Invalid color value: %d", color);
        return ERR_INVALID_PARAM;
    }

    if (brightness > LED_FULL) {
        LOG_ERR("Invalid brightness value: %d", brightness);
        return ERR_INVALID_PARAM;
    }

    gpio_pin_set_dt(&led_red,   (color == LED_COLOR_RED) && (brightness > 0));
    gpio_pin_set_dt(&led_green, (color == LED_COLOR_GREEN) && (brightness > 0));
    gpio_pin_set_dt(&led_blue,  (color == LED_COLOR_BLUE) && (brightness > 0));
    
    current_color = color;
    current_brightness = brightness;
    LOG_INF("LED color set to %d with brightness %d", color, brightness);
    return ERR_NONE;
}

error_code_t led_fade_to_color(led_color_t color, uint32_t fade_time_ms)
{
    if (color >= LED_COLOR_MAX) {
        LOG_ERR("Invalid color value: %d", color);
        return ERR_INVALID_PARAM;
    }

    // TODO: Implement PWM-based fade effect
    LOG_INF("Fade to color %d over %d ms", color, fade_time_ms);
    return led_set_color(color, LED_FULL);
}

error_code_t led_init(void)
{
    if (!device_is_ready(led_red.port) ||
        !device_is_ready(led_green.port) ||
        !device_is_ready(led_blue.port)) {
        LOG_ERR("LED devices not ready");
        return ERR_DEVICE_NOT_READY;
    }

    gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE);

    LOG_INF("LEDs initialized");
    return ERR_NONE;
} 