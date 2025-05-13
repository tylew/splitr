#ifndef GPIO_CONTROLS_H
#define GPIO_CONTROLS_H

#include "esp_err.h"

// GPIO pins for buttons
#define GPIO_PLAY_PAUSE_BTN   32  // Play/Pause button
#define GPIO_STOP_BTN         33  // Stop button
#define GPIO_VOL_UP_BTN       25  // Volume up button
#define GPIO_VOL_DOWN_BTN     26  // Volume down button

// Button debounce time in milliseconds
#define BUTTON_DEBOUNCE_TIME_MS   50

// Button rate limiting in milliseconds (minimum time between valid presses)
#define BUTTON_RATE_LIMIT_MS      200

// Volume step percentage for each button press
#define VOLUME_STEP_PERCENT       5

/**
 * @brief Initialize GPIO button controls
 * 
 * Configures the GPIO pins for the buttons and sets up the interrupt handlers
 * 
 * @return esp_err_t ESP_OK if initialization successful, error code otherwise
 */
esp_err_t gpio_controls_init(void);

/**
 * @brief Deinitialize GPIO button controls
 * 
 * Removes interrupt handlers and frees resources
 * 
 * @return esp_err_t ESP_OK if deinitialization successful, error code otherwise
 */
esp_err_t gpio_controls_deinit(void);

#endif /* GPIO_CONTROLS_H */ 