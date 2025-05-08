/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate RGB LED control using LED module
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "utils/led.h"
#include "utils/macros/macros_common.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

int main(void)
{
	int ret;

	printk("RGB LED control using LED module\n");

	ret = led_init();
	ERR_CHK_MSG(ret, "LED module initialization failed");

	while (1) {
		// Cycle through basic colors
		ret = led_on(LED_APP_RGB, LED_COLOR_RED);
		ERR_CHK(ret);
		k_sleep(K_SECONDS(1));
		
		ret = led_on(LED_APP_RGB, LED_COLOR_GREEN);
		ERR_CHK(ret);
		k_sleep(K_SECONDS(1));
		
		ret = led_on(LED_APP_RGB, LED_COLOR_BLUE);
		ERR_CHK(ret);
		k_sleep(K_SECONDS(1));
		
		ret = led_on(LED_APP_RGB, LED_COLOR_WHITE);
		ERR_CHK(ret);
		k_sleep(K_SECONDS(1));
		
		ret = led_off(LED_APP_RGB);
		ERR_CHK(ret);
		k_sleep(K_SECONDS(1));
	}
	
	return 0;
}
