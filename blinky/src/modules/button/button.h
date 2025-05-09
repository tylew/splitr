#ifndef MODULES_BUTTON_H
#define MODULES_BUTTON_H

#include "../../utils/common.h"

/* Button event types */
typedef enum {
    BUTTON_PRESSED = 0,
    BUTTON_RELEASED,
    BUTTON_HELD
} button_event_t;

/* Button callback function type */
typedef void (*button_callback_t)(button_event_t event);

/* Initialize button hardware and callbacks */
error_code_t button_init(void);

/* Register a callback for button events */
error_code_t button_register_callback(button_callback_t callback);

#endif /* MODULES_BUTTON_H */ 