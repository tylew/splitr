#include "audio_player.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/i2s.h"
#include "esp_spiffs.h"
#include <string.h>

// libhelix MP3 decoder headers
#include "mp3dec.h"

#define TAG "AUDIO_PLAYER"

// MP3 buffer sizes
#define MP3_TASK_STACK_SIZE     4096
#define MP3_TASK_PRIORITY       5
#define MP3_READ_BUFFER_SIZE    1024    // Size of buffer for reading MP3 data
#define PCM_BUFFER_SIZE         2048    // Size of buffer for decoded PCM data (must be big enough for a frame)

// I2S configuration
#define I2S_SAMPLE_RATE         44100   // Default sample rate (will be adjusted based on MP3)
#define I2S_BITS_PER_SAMPLE     16      // 16-bit audio
#define I2S_NUM_CHANNELS        2       // Stereo

// Player state
static bool s_player_initialized = false;
static bool s_player_playing = false;
static TaskHandle_t s_player_task_handle = NULL;
static int s_i2s_port = 0;
static SemaphoreHandle_t s_player_mutex = NULL;

// File state
static FILE* s_mp3_file = NULL;
static char s_filepath[128] = {0};

// Memory playback state
static const uint8_t* s_mp3_data = NULL;
static size_t s_mp3_data_len = 0;
static size_t s_mp3_data_pos = 0;
static bool s_playing_from_memory = false;

// Decoder state
static HMP3Decoder s_mp3_decoder = NULL;
static uint8_t s_mp3_read_buffer[MP3_READ_BUFFER_SIZE];
static int16_t s_pcm_buffer[PCM_BUFFER_SIZE];

// Forward declarations
static void mp3_player_task(void *pvParameters);
static void mp3_memory_player_task(void *pvParameters);
static esp_err_t init_i2s(int sample_rate, int channels);

esp_err_t audio_player_init(int i2s_port) {
    if (s_player_initialized) {
        ESP_LOGW(TAG, "Player already initialized");
        return ESP_OK;
    }

    // Initialize SPIFFS for MP3 file storage
    esp_vfs_spiffs_conf_t spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Initialize I2S with default values (will be reconfigured when we start playing)
    s_i2s_port = i2s_port;
    ret = init_i2s(I2S_SAMPLE_RATE, I2S_NUM_CHANNELS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2S (%s)", esp_err_to_name(ret));
        esp_vfs_spiffs_unregister(NULL);
        return ret;
    }

    // Create mutex for player state
    s_player_mutex = xSemaphoreCreateMutex();
    if (s_player_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create player mutex");
        i2s_driver_uninstall(s_i2s_port);
        esp_vfs_spiffs_unregister(NULL);
        return ESP_ERR_NO_MEM;
    }

    // Initialize MP3 decoder
    s_mp3_decoder = MP3InitDecoder();
    if (s_mp3_decoder == NULL) {
        ESP_LOGE(TAG, "Failed to initialize MP3 decoder");
        vSemaphoreDelete(s_player_mutex);
        i2s_driver_uninstall(s_i2s_port);
        esp_vfs_spiffs_unregister(NULL);
        return ESP_ERR_NO_MEM;
    }

    s_player_initialized = true;
    ESP_LOGI(TAG, "Audio player initialized successfully");
    return ESP_OK;
}

esp_err_t audio_player_play_mp3(const char* filepath) {
    if (!s_player_initialized) {
        ESP_LOGE(TAG, "Player not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Take the mutex to modify player state
    if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take player mutex");
        return ESP_ERR_TIMEOUT;
    }

    // If already playing, stop current playback
    if (s_player_playing) {
        xSemaphoreGive(s_player_mutex);
        audio_player_stop();
        xSemaphoreTake(s_player_mutex, portMAX_DELAY);
    }

    // Construct full path with /spiffs prefix
    char full_path[140];
    if (filepath[0] == '/') {
        snprintf(full_path, sizeof(full_path), "/spiffs%s", filepath);
    } else {
        snprintf(full_path, sizeof(full_path), "/spiffs/%s", filepath);
    }

    // Open the MP3 file
    s_mp3_file = fopen(full_path, "rb");
    if (s_mp3_file == NULL) {
        ESP_LOGE(TAG, "Failed to open MP3 file: %s", full_path);
        xSemaphoreGive(s_player_mutex);
        return ESP_ERR_NOT_FOUND;
    }

    // Save the filepath for reference
    strncpy(s_filepath, filepath, sizeof(s_filepath) - 1);
    s_filepath[sizeof(s_filepath) - 1] = '\0';
    
    // Mark that we're playing from file, not memory
    s_playing_from_memory = false;

    // Create MP3 player task
    BaseType_t task_created = xTaskCreate(
        mp3_player_task,
        "mp3_player_task",
        MP3_TASK_STACK_SIZE,
        NULL,
        MP3_TASK_PRIORITY,
        &s_player_task_handle
    );

    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create MP3 player task");
        fclose(s_mp3_file);
        s_mp3_file = NULL;
        xSemaphoreGive(s_player_mutex);
        return ESP_ERR_NO_MEM;
    }

    s_player_playing = true;
    xSemaphoreGive(s_player_mutex);
    
    ESP_LOGI(TAG, "Started playing MP3 file: %s", filepath);
    return ESP_OK;
}

esp_err_t audio_player_play_mp3_data(const uint8_t* data, size_t len) {
    if (!s_player_initialized) {
        ESP_LOGE(TAG, "Player not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || len == 0) {
        ESP_LOGE(TAG, "Invalid MP3 data pointer or length");
        return ESP_ERR_INVALID_ARG;
    }

    // Take the mutex to modify player state
    if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take player mutex");
        return ESP_ERR_TIMEOUT;
    }

    // If already playing, stop current playback
    if (s_player_playing) {
        xSemaphoreGive(s_player_mutex);
        audio_player_stop();
        xSemaphoreTake(s_player_mutex, portMAX_DELAY);
    }

    // Store the MP3 data pointer and length
    s_mp3_data = data;
    s_mp3_data_len = len;
    s_mp3_data_pos = 0;
    s_playing_from_memory = true;

    // Create MP3 memory player task
    BaseType_t task_created = xTaskCreate(
        mp3_memory_player_task,
        "mp3_mem_player_task",
        MP3_TASK_STACK_SIZE,
        NULL,
        MP3_TASK_PRIORITY,
        &s_player_task_handle
    );

    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create MP3 memory player task");
        s_mp3_data = NULL;
        s_mp3_data_len = 0;
        xSemaphoreGive(s_player_mutex);
        return ESP_ERR_NO_MEM;
    }

    s_player_playing = true;
    xSemaphoreGive(s_player_mutex);
    
    ESP_LOGI(TAG, "Started playing MP3 data from memory, size: %d bytes", len);
    return ESP_OK;
}

esp_err_t audio_player_stop(void) {
    if (!s_player_initialized) {
        ESP_LOGE(TAG, "Player not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Take the mutex to modify player state
    if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take player mutex");
        return ESP_ERR_TIMEOUT;
    }

    if (!s_player_playing) {
        xSemaphoreGive(s_player_mutex);
        return ESP_OK;  // Already stopped
    }

    // Signal the task to stop and wait for it to end
    s_player_playing = false;
    xSemaphoreGive(s_player_mutex);

    // Wait for the task to finish
    if (s_player_task_handle != NULL) {
        vTaskDelay(pdMS_TO_TICKS(100));  // Give task a chance to close gracefully
        vTaskDelete(s_player_task_handle);
        s_player_task_handle = NULL;
    }

    // Take the mutex again to safely clean up
    xSemaphoreTake(s_player_mutex, portMAX_DELAY);

    // Close the file if open
    if (s_mp3_file != NULL) {
        fclose(s_mp3_file);
        s_mp3_file = NULL;
    }

    // Clear memory playback state
    s_mp3_data = NULL;
    s_mp3_data_len = 0;
    s_mp3_data_pos = 0;
    s_playing_from_memory = false;

    xSemaphoreGive(s_player_mutex);
    ESP_LOGI(TAG, "Stopped MP3 playback");
    return ESP_OK;
}

bool audio_player_is_playing(void) {
    if (!s_player_initialized) {
        return false;
    }

    bool playing = false;
    if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) == pdTRUE) {
        playing = s_player_playing;
        xSemaphoreGive(s_player_mutex);
    }
    return playing;
}

esp_err_t audio_player_deinit(void) {
    if (!s_player_initialized) {
        return ESP_OK;  // Already deinitialized
    }

    // Stop any active playback
    audio_player_stop();

    // Take the mutex to safely deinitialize
    if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take player mutex");
        return ESP_ERR_TIMEOUT;
    }

    // Free the MP3 decoder
    if (s_mp3_decoder != NULL) {
        MP3FreeDecoder(s_mp3_decoder);
        s_mp3_decoder = NULL;
    }

    xSemaphoreGive(s_player_mutex);
    vSemaphoreDelete(s_player_mutex);
    s_player_mutex = NULL;

    // Uninstall I2S driver
    i2s_driver_uninstall(s_i2s_port);

    // Unmount SPIFFS
    esp_vfs_spiffs_unregister(NULL);

    s_player_initialized = false;
    ESP_LOGI(TAG, "Audio player deinitialized");
    return ESP_OK;
}

/**
 * MP3 player task - Decodes MP3 and streams to I2S
 */
static void mp3_player_task(void *pvParameters) {
    ESP_LOGI(TAG, "MP3 player task started");

    int bytes_read = 0;
    int bytes_remaining = 0;
    uint8_t *read_ptr = s_mp3_read_buffer;
    int offset = 0;
    int err = 0;
    MP3FrameInfo frame_info;
    bool first_frame = true;
    int sample_rate = 0;
    int channels = 0;

    while (1) {
        // Check if we should stop
        if (xSemaphoreTake(s_player_mutex, 0) == pdTRUE) {
            if (!s_player_playing) {
                xSemaphoreGive(s_player_mutex);
                break;
            }
            xSemaphoreGive(s_player_mutex);
        }

        // Fill the read buffer if needed
        if (bytes_remaining < MP3_READ_BUFFER_SIZE / 2) {
            // Move remaining data to beginning of buffer
            if (bytes_remaining > 0) {
                memmove(s_mp3_read_buffer, read_ptr, bytes_remaining);
            }
            
            read_ptr = s_mp3_read_buffer;
            
            // Read more MP3 data
            bytes_read = fread(s_mp3_read_buffer + bytes_remaining, 1, 
                              MP3_READ_BUFFER_SIZE - bytes_remaining, s_mp3_file);
            
            if (bytes_read == 0) {
                // End of file - stop playback
                ESP_LOGI(TAG, "End of MP3 file reached");
                
                if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) == pdTRUE) {
                    s_player_playing = false;
                    xSemaphoreGive(s_player_mutex);
                }
                break;
            }
            
            bytes_remaining += bytes_read;
        }

        // Find MP3 sync word (frame header)
        offset = MP3FindSyncWord(read_ptr, bytes_remaining);
        if (offset < 0) {
            // No sync word found, need more data
            ESP_LOGW(TAG, "No sync word found, reading more data");
            bytes_remaining = 0;  // Force buffer refill
            continue;
        }

        // Adjust read pointer and remaining bytes
        read_ptr += offset;
        bytes_remaining -= offset;

        // Decode MP3 frame
        err = MP3Decode(s_mp3_decoder, &read_ptr, &bytes_remaining, s_pcm_buffer, 0);
        if (err) {
            ESP_LOGW(TAG, "MP3 decode error %d, skipping frame", err);
            continue;
        }

        // Get frame info
        MP3GetLastFrameInfo(s_mp3_decoder, &frame_info);

        // If this is the first valid frame, reconfigure I2S with actual sample rate
        if (first_frame) {
            sample_rate = frame_info.samprate;
            channels = frame_info.nChans;
            
            ESP_LOGI(TAG, "MP3 info: %d Hz, %d channels, %d bits, %d samples per frame",
                     frame_info.samprate, frame_info.nChans, frame_info.bitsPerSample, frame_info.outputSamps);
            
            // Reconfigure I2S with the actual sample rate from MP3
            init_i2s(sample_rate, channels);
            
            first_frame = false;
        }

        // Write PCM data to I2S
        size_t bytes_written = 0;
        i2s_write(s_i2s_port, s_pcm_buffer, frame_info.outputSamps * frame_info.nChans * sizeof(int16_t), 
                  &bytes_written, portMAX_DELAY);
                  
        // Handle underflow/overflow
        if (bytes_written < frame_info.outputSamps * frame_info.nChans * sizeof(int16_t)) {
            ESP_LOGW(TAG, "I2S buffer underflow");
        }
    }

    // Close the file when task exits
    if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) == pdTRUE) {
        if (s_mp3_file != NULL) {
            fclose(s_mp3_file);
            s_mp3_file = NULL;
        }
        xSemaphoreGive(s_player_mutex);
    }

    ESP_LOGI(TAG, "MP3 player task ended");
    vTaskDelete(NULL);
}

/**
 * MP3 memory player task - Decodes MP3 from memory and streams to I2S
 */
static void mp3_memory_player_task(void *pvParameters) {
    ESP_LOGI(TAG, "MP3 memory player task started");

    int bytes_remaining = 0;
    uint8_t *read_ptr = s_mp3_read_buffer;
    int offset = 0;
    int err = 0;
    MP3FrameInfo frame_info;
    bool first_frame = true;
    int sample_rate = 0;
    int channels = 0;

    while (1) {
        // Check if we should stop
        if (xSemaphoreTake(s_player_mutex, 0) == pdTRUE) {
            if (!s_player_playing) {
                xSemaphoreGive(s_player_mutex);
                break;
            }
            xSemaphoreGive(s_player_mutex);
        }

        // Fill the read buffer if needed
        if (bytes_remaining < MP3_READ_BUFFER_SIZE / 2) {
            // Move remaining data to beginning of buffer
            if (bytes_remaining > 0) {
                memmove(s_mp3_read_buffer, read_ptr, bytes_remaining);
            }
            
            read_ptr = s_mp3_read_buffer;
            
            // Copy more data from memory
            if (xSemaphoreTake(s_player_mutex, portMAX_DELAY) == pdTRUE) {
                // Check if we still have data to copy
                size_t bytes_to_copy = MP3_READ_BUFFER_SIZE - bytes_remaining;
                if (s_mp3_data_pos + bytes_to_copy > s_mp3_data_len) {
                    bytes_to_copy = s_mp3_data_len - s_mp3_data_pos;
                }
                
                if (bytes_to_copy > 0) {
                    // Copy data from the memory buffer
                    memcpy(s_mp3_read_buffer + bytes_remaining, s_mp3_data + s_mp3_data_pos, bytes_to_copy);
                    s_mp3_data_pos += bytes_to_copy;
                    bytes_remaining += bytes_to_copy;
                } else {
                    // End of memory buffer - stop playback
                    ESP_LOGI(TAG, "End of MP3 memory data reached");
                    s_player_playing = false;
                    xSemaphoreGive(s_player_mutex);
                    break;
                }
                
                xSemaphoreGive(s_player_mutex);
            }
        }

        // Find MP3 sync word (frame header)
        offset = MP3FindSyncWord(read_ptr, bytes_remaining);
        if (offset < 0) {
            // No sync word found, need more data
            ESP_LOGW(TAG, "No sync word found, reading more data");
            bytes_remaining = 0;  // Force buffer refill
            continue;
        }

        // Adjust read pointer and remaining bytes
        read_ptr += offset;
        bytes_remaining -= offset;

        // Decode MP3 frame
        err = MP3Decode(s_mp3_decoder, &read_ptr, &bytes_remaining, s_pcm_buffer, 0);
        if (err) {
            ESP_LOGW(TAG, "MP3 decode error %d, skipping frame", err);
            continue;
        }

        // Get frame info
        MP3GetLastFrameInfo(s_mp3_decoder, &frame_info);

        // If this is the first valid frame, reconfigure I2S with actual sample rate
        if (first_frame) {
            sample_rate = frame_info.samprate;
            channels = frame_info.nChans;
            
            ESP_LOGI(TAG, "MP3 info: %d Hz, %d channels, %d bits, %d samples per frame",
                     frame_info.samprate, frame_info.nChans, frame_info.bitsPerSample, frame_info.outputSamps);
            
            // Reconfigure I2S with the actual sample rate from MP3
            init_i2s(sample_rate, channels);
            
            first_frame = false;
        }

        // Write PCM data to I2S
        size_t bytes_written = 0;
        i2s_write(s_i2s_port, s_pcm_buffer, frame_info.outputSamps * frame_info.nChans * sizeof(int16_t), 
                  &bytes_written, portMAX_DELAY);
                  
        // Handle underflow/overflow
        if (bytes_written < frame_info.outputSamps * frame_info.nChans * sizeof(int16_t)) {
            ESP_LOGW(TAG, "I2S buffer underflow");
        }
    }

    ESP_LOGI(TAG, "MP3 memory player task ended");
    vTaskDelete(NULL);
}

/**
 * Initialize I2S driver with given parameters
 */
static esp_err_t init_i2s(int sample_rate, int channels) {
    // First uninstall if previously installed
    i2s_driver_uninstall(s_i2s_port);
    
    // Configure I2S with specified parameters
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = sample_rate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = (channels == 1) ? I2S_CHANNEL_FMT_ONLY_RIGHT : I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    };
    
    // I2S pins configuration
    i2s_pin_config_t pin_config = {
        .bck_io_num = 26,    // Bit clock pin
        .ws_io_num = 25,     // Word select pin
        .data_out_num = 22,  // Data output pin
        .data_in_num = -1    // Not used for output
    };
    
    // Install I2S driver
    esp_err_t ret = i2s_driver_install(s_i2s_port, &i2s_config, 0, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver: %d", ret);
        return ret;
    }
    
    // Set I2S pins
    ret = i2s_set_pin(s_i2s_port, &pin_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pins: %d", ret);
        i2s_driver_uninstall(s_i2s_port);
        return ret;
    }
    
    ESP_LOGI(TAG, "I2S initialized - rate: %d Hz, channels: %d", sample_rate, channels);
    return ESP_OK;
} 