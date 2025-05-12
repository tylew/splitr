#include "gpio_controls.h"
#include "audio_player.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char *TAG = "GPIO_CONTROLS";

// Button events queue
static QueueHandle_t s_button_event_queue = NULL;

// Button state tracking
typedef enum {
    BUTTON_PLAY_PAUSE = 0,
    BUTTON_STOP,
    BUTTON_VOLUME_UP,
    BUTTON_VOLUME_DOWN,
    BUTTON_COUNT
} button_id_t;

typedef struct {
    button_id_t button_id;
    uint64_t pressed_time;
} button_event_t;

// Button last pressed timestamps (for rate limiting)
static uint64_t s_last_press_time[BUTTON_COUNT] = {0};

// Button processing task handle
static TaskHandle_t s_button_task_handle = NULL;

// Forward declarations
static void IRAM_ATTR gpio_button_isr_handler(void* arg);
static void button_processing_task(void* arg);

esp_err_t gpio_controls_init(void) {
    esp_err_t ret = ESP_OK;
    
    ESP_LOGI(TAG, "Initializing GPIO button controls");
    
    // Create button event queue
    s_button_event_queue = xQueueCreate(10, sizeof(button_event_t));
    if (s_button_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Configure GPIO pins for buttons with internal pull-ups
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,     // Interrupt on falling edge (button press)
        .mode = GPIO_MODE_INPUT,            // Input mode
        .pin_bit_mask = (1ULL << GPIO_PLAY_PAUSE_BTN) | 
                        (1ULL << GPIO_STOP_BTN) | 
                        (1ULL << GPIO_VOL_UP_BTN) | 
                        (1ULL << GPIO_VOL_DOWN_BTN),
        .pull_up_en = GPIO_PULLUP_ENABLE,   // Enable pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    
    ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO pins: %d", ret);
        vQueueDelete(s_button_event_queue);
        s_button_event_queue = NULL;
        return ret;
    }
    
    // Install GPIO ISR service
    ret = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        // ESP_ERR_INVALID_STATE means ISR service already installed, which is OK
        ESP_LOGE(TAG, "Failed to install GPIO ISR service: %d", ret);
        vQueueDelete(s_button_event_queue);
        s_button_event_queue = NULL;
        return ret;
    }
    
    // Add ISR handlers for each button
    gpio_isr_handler_add(GPIO_PLAY_PAUSE_BTN, gpio_button_isr_handler, (void*)BUTTON_PLAY_PAUSE);
    gpio_isr_handler_add(GPIO_STOP_BTN, gpio_button_isr_handler, (void*)BUTTON_STOP);
    gpio_isr_handler_add(GPIO_VOL_UP_BTN, gpio_button_isr_handler, (void*)BUTTON_VOLUME_UP);
    gpio_isr_handler_add(GPIO_VOL_DOWN_BTN, gpio_button_isr_handler, (void*)BUTTON_VOLUME_DOWN);
    
    // Create button processing task
    BaseType_t task_created = xTaskCreate(
        button_processing_task,
        "button_task",
        2048,
        NULL,
        10,  // Higher priority than audio tasks
        &s_button_task_handle
    );
    
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create button processing task");
        gpio_isr_handler_remove(GPIO_PLAY_PAUSE_BTN);
        gpio_isr_handler_remove(GPIO_STOP_BTN);
        gpio_isr_handler_remove(GPIO_VOL_UP_BTN);
        gpio_isr_handler_remove(GPIO_VOL_DOWN_BTN);
        vQueueDelete(s_button_event_queue);
        s_button_event_queue = NULL;
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "GPIO button controls initialized successfully");
    return ESP_OK;
}

esp_err_t gpio_controls_deinit(void) {
    ESP_LOGI(TAG, "Deinitializing GPIO button controls");
    
    // Remove ISR handlers
    gpio_isr_handler_remove(GPIO_PLAY_PAUSE_BTN);
    gpio_isr_handler_remove(GPIO_STOP_BTN);
    gpio_isr_handler_remove(GPIO_VOL_UP_BTN);
    gpio_isr_handler_remove(GPIO_VOL_DOWN_BTN);
    
    // Delete button processing task
    if (s_button_task_handle != NULL) {
        vTaskDelete(s_button_task_handle);
        s_button_task_handle = NULL;
    }
    
    // Delete event queue
    if (s_button_event_queue != NULL) {
        vQueueDelete(s_button_event_queue);
        s_button_event_queue = NULL;
    }
    
    ESP_LOGI(TAG, "GPIO button controls deinitialized");
    return ESP_OK;
}

/**
 * ISR handler for button presses
 * Sends button events to the processing queue
 */
static void IRAM_ATTR gpio_button_isr_handler(void* arg) {
    button_id_t button_id = (button_id_t)arg;
    button_event_t event;
    event.button_id = button_id;
    event.pressed_time = esp_timer_get_time() / 1000; // Convert to milliseconds
    
    // Send button event to queue
    xQueueSendFromISR(s_button_event_queue, &event, NULL);
}

/**
 * Button processing task
 * Handles debouncing and executes button functions
 */
static void button_processing_task(void* arg) {
    button_event_t event;
    
    ESP_LOGI(TAG, "Button processing task started");
    
    while (1) {
        if (xQueueReceive(s_button_event_queue, &event, portMAX_DELAY)) {
            // Get current time
            uint64_t current_time = esp_timer_get_time() / 1000;
            
            // Rate limiting check - only process button if enough time has passed since last press
            if ((current_time - s_last_press_time[event.button_id]) < BUTTON_RATE_LIMIT_MS) {
                ESP_LOGD(TAG, "Button %d press ignored (rate limiting)", event.button_id);
                continue;
            }
            
            // Debounce - wait a short period then check if button is still pressed
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_TIME_MS));
            
            // Check if button is still pressed after debounce delay
            bool button_state = false;
            switch (event.button_id) {
                case BUTTON_PLAY_PAUSE:
                    button_state = !gpio_get_level(GPIO_PLAY_PAUSE_BTN);
                    break;
                case BUTTON_STOP:
                    button_state = !gpio_get_level(GPIO_STOP_BTN);
                    break;
                case BUTTON_VOLUME_UP:
                    button_state = !gpio_get_level(GPIO_VOL_UP_BTN);
                    break;
                case BUTTON_VOLUME_DOWN:
                    button_state = !gpio_get_level(GPIO_VOL_DOWN_BTN);
                    break;
                default:
                    button_state = false;
                    break;
            }
            
            // If button released too quickly (noise), ignore event
            if (!button_state) {
                ESP_LOGD(TAG, "Button %d press ignored (debounce)", event.button_id);
                continue;
            }
            
            // Update last press time for this button
            s_last_press_time[event.button_id] = current_time;
            
            // Process the button press
            switch (event.button_id) {
                case BUTTON_PLAY_PAUSE:
                    ESP_LOGI(TAG, "Play/Pause button pressed");
                    if (audio_player_is_playing()) {
                        audio_player_pause_resume();
                    } else {
                        // If nothing is playing, try to resume or start a default playback
                        // Could add default playback logic here if needed
                        audio_player_pause_resume();
                    }
                    break;
                    
                case BUTTON_STOP:
                    ESP_LOGI(TAG, "Stop button pressed");
                    audio_player_stop();
                    break;
                    
                case BUTTON_VOLUME_UP:
                    ESP_LOGI(TAG, "Volume Up button pressed");
                    audio_player_volume_up(VOLUME_STEP_PERCENT);
                    break;
                    
                case BUTTON_VOLUME_DOWN:
                    ESP_LOGI(TAG, "Volume Down button pressed");
                    audio_player_volume_down(VOLUME_STEP_PERCENT);
                    break;
                    
                default:
                    ESP_LOGW(TAG, "Unknown button ID: %d", event.button_id);
                    break;
            }
        }
    }
} 