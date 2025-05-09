#ifndef MODULES_LED_H
#define MODULES_LED_H

#include "../../utils/common.h"

/* LED brightness levels */
typedef enum {
    LED_OFF = 0,
    LED_LOW = 85,
    LED_MEDIUM = 170,
    LED_FULL = 255
} led_brightness_t;

/* Initialize LED hardware */
error_code_t led_init(void);

/* Set LED color with optional brightness */
error_code_t led_set_color(led_color_t color, led_brightness_t brightness);

/* Set LED color with fade effect */
error_code_t led_fade_to_color(led_color_t color, uint32_t fade_time_ms);

#endif /* MODULES_LED_H */ 