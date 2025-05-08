#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/**
 * @brief Initialize the MP3 player with I2S output
 * 
 * @param i2s_port The I2S port number to use (typically 0)
 * @return esp_err_t ESP_OK if successfully initialized, error code otherwise
 */
esp_err_t audio_player_init(int i2s_port);

/**
 * @brief Play an MP3 file stored in SPIFFS
 * 
 * @param filepath Path to the MP3 file in SPIFFS
 * @return esp_err_t ESP_OK if successfully started playback, error code otherwise
 */
esp_err_t audio_player_play_mp3(const char* filepath);

/**
 * @brief Play MP3 data from memory buffer
 * 
 * @param data Pointer to MP3 data buffer
 * @param len Length of MP3 data in bytes
 * @return esp_err_t ESP_OK if successfully started playback, error code otherwise
 */
esp_err_t audio_player_play_mp3_data(const uint8_t* data, size_t len);

/**
 * @brief Stop the current playback
 * 
 * @return esp_err_t ESP_OK if successfully stopped, error code otherwise
 */
esp_err_t audio_player_stop(void);

/**
 * @brief Check if player is currently playing
 * 
 * @return true if player is active, false otherwise
 */
bool audio_player_is_playing(void);

/**
 * @brief Cleanup and release all resources used by audio player
 * 
 * @return esp_err_t ESP_OK if successfully cleaned up, error code otherwise
 */
esp_err_t audio_player_deinit(void);

#endif /* AUDIO_PLAYER_H */ 