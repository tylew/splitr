#include "led.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led, LOG_LEVEL_INF);

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

static int current_color = 0;

void led_set_color(int color)
{
    if (color < 0 || color > 2) {
        LOG_ERR("Invalid color value: %d", color);
        return;
    }

    gpio_pin_set_dt(&led_red,   color == 0);
    gpio_pin_set_dt(&led_green, color == 1);
    gpio_pin_set_dt(&led_blue,  color == 2);
    
    current_color = color;
    LOG_INF("LED color set to %d", color);
}

int led_init(void)
{
    if (!device_is_ready(led_red.port) ||
        !device_is_ready(led_green.port) ||
        !device_is_ready(led_blue.port)) {
        LOG_ERR("LED devices not ready");
        return -ENODEV;
    }

    gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE);

    LOG_INF("LEDs initialized");
    return 0;
} 