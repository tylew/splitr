#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define BUTTON5_NODE    DT_ALIAS(sw4)
#define LED_RED_NODE    DT_ALIAS(led0)
#define LED_GREEN_NODE  DT_ALIAS(led1)
#define LED_BLUE_NODE   DT_ALIAS(led2)

#define DEBOUNCE_MS 50

static const struct gpio_dt_spec button5 = GPIO_DT_SPEC_GET_OR(BUTTON5_NODE, gpios, {0});
static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

static struct gpio_callback button_cb_data;
static int current_color = 0;

static void update_leds(int color)
{
    gpio_pin_set_dt(&led_red,   color == 0);
    gpio_pin_set_dt(&led_green, color == 1);
    gpio_pin_set_dt(&led_blue,  color == 2);
}

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    static int64_t last_press = 0;
    int64_t now = k_uptime_get();

    if (now - last_press < DEBOUNCE_MS) return;
    last_press = now;

    current_color = (current_color + 1) % 3;
    update_leds(current_color);
    printk("Color changed to %d\n", current_color);
}

int main(void)
{
    if (!device_is_ready(button5.port) ||
        !device_is_ready(led_red.port) ||
        !device_is_ready(led_green.port) ||
        !device_is_ready(led_blue.port)) {
        printk("Error: device not ready.\n");
        return 1;
    }

    gpio_pin_configure_dt(&button5, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button5, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button5.pin));
    gpio_add_callback(button5.port, &button_cb_data);

    gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE);

    printk("Ready to rumble with RGB control on Button 5\n");

    while (1) {
        k_sleep(K_FOREVER); // Let ISR handle everything
    }
}
