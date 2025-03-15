/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "bt_mgmt.h"
#include "fw_info_app.h"
#include "nrf5340_audio_common.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(audio_common, CONFIG_LOG_DEFAULT_LEVEL);

static struct gpio_callback button_cb_data;
static button_handler_cb button_callback;

static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	if (button_callback) {
		button_callback(pins, pins);
	}
}

int button_handler_init(button_handler_cb callback)
{
	const struct device *gpio_dev;
	int ret;

	if (!callback) {
		return -EINVAL;
	}

	button_callback = callback;

	gpio_dev = device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(gpio0), gpios));
	if (!gpio_dev) {
		LOG_ERR("GPIO device not found");
		return -ENODEV;
	}

	ret = gpio_pin_configure(gpio_dev, CONFIG_BUTTON_1, GPIO_INPUT | GPIO_PULL_UP);
	if (ret) {
		LOG_ERR("Failed to configure button pin");
		return ret;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(CONFIG_BUTTON_1));
	ret = gpio_add_callback(gpio_dev, &button_cb_data);
	if (ret) {
		LOG_ERR("Failed to add button callback");
		return ret;
	}

	ret = gpio_pin_interrupt_configure(gpio_dev, CONFIG_BUTTON_1, GPIO_INT_EDGE_BOTH);
	if (ret) {
		LOG_ERR("Failed to configure button interrupt");
		return ret;
	}

	return 0;
}

int nrf5340_audio_common_init(void)
{
	int ret;

	ret = fw_info_app_print();
	if (ret) {
		LOG_ERR("Failed to print application FW info");
		return ret;
	}

	ret = bt_mgmt_init();
	if (ret) {
		LOG_ERR("Failed to initialize bt_mgmt");
		return ret;
	}

	return 0;
}
