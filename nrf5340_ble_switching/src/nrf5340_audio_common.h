#ifndef _NRF5340_AUDIO_COMMON_H_
#define _NRF5340_AUDIO_COMMON_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Button assignments structure */
struct button_assignments {
    uint32_t mode_switch;
};

/* Button handler callback type */
typedef void (*button_handler_cb)(uint32_t button_pin, uint32_t has_changed);

/* Initialize button handling */
int button_handler_init(button_handler_cb callback);

#endif /* _NRF5340_AUDIO_COMMON_H_ */ 