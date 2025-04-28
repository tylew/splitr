#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "audio_player.h"
#include "sample_mp3.h"

#define EXAMPLE_ESP_WIFI_SSID      "ESP32-Access-Point"
#define EXAMPLE_ESP_WIFI_PASS      "123456789"
#define EXAMPLE_MAX_STA_CONN       4

// UART configuration
#define UART_PORT UART_NUM_1
#define UART_TX_PIN 17
#define UART_RX_PIN 16
#define UART_BAUD_RATE 115200

// I2S port number
#define I2S_PORT_NUM 0

static const char *TAG = "esp32_ap_api";

// Helper function to send UART commands
static void send_uart_command(const char* cmd)
{
    uart_write_bytes(UART_PORT, cmd, strlen(cmd));
    ESP_LOGI(TAG, "Sent UART command: %s", cmd);
}

void wifi_init_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("wifi_init_softap", "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

static esp_err_t data_get_handler(httpd_req_t *req) {
    char resp_str[100];
    
    // Get the URI
    char uri[100];
    strlcpy(uri, req->uri, sizeof(uri));
    
    // Check the URI and send appropriate UART command
    if (strcmp(uri, "/api/play") == 0) {
        send_uart_command("PLAY\n");
        strcpy(resp_str, "{\"status\": \"Playing audio\"}");
    } 
    else if (strcmp(uri, "/api/pause") == 0) {
        send_uart_command("PAUSE\n");
        strcpy(resp_str, "{\"status\": \"Paused audio\"}");
    }
    else if (strncmp(uri, "/api/volume/", 12) == 0) {
        // Extract volume value
        int volume = atoi(uri + 12);
        // Ensure volume is within valid range (0-100)
        if (volume >= 0 && volume <= 100) {
            char vol_cmd[20];
            sprintf(vol_cmd, "VOL:%d\n", volume);
            send_uart_command(vol_cmd);
            
            sprintf(resp_str, "{\"status\": \"Volume set to %d\"}", volume);
        } else {
            strcpy(resp_str, "{\"error\": \"Invalid volume level (should be 0-100)\"}");
        }
    }
    else if (strcmp(uri, "/api/data") == 0) {
        // Original data endpoint
        strcpy(resp_str, "{\"message\": \"Hello, World!\"}");
    }
    // New MP3 playback endpoints
    else if (strcmp(uri, "/api/mp3/play") == 0) {
        // Start MP3 playback of sample file
        esp_err_t ret = audio_player_play_mp3("/sample.mp3");
        if (ret == ESP_OK) {
            strcpy(resp_str, "{\"status\": \"Started MP3 playback\"}");
        } else {
            sprintf(resp_str, "{\"error\": \"Failed to start MP3 playback: %s\"}", esp_err_to_name(ret));
        }
    }
    else if (strcmp(uri, "/api/mp3/stop") == 0) {
        // Stop MP3 playback
        esp_err_t ret = audio_player_stop();
        if (ret == ESP_OK) {
            strcpy(resp_str, "{\"status\": \"Stopped MP3 playback\"}");
        } else {
            sprintf(resp_str, "{\"error\": \"Failed to stop MP3 playback: %s\"}", esp_err_to_name(ret));
        }
    }
    else if (strcmp(uri, "/api/mp3/play-embedded") == 0) {
        // Play MP3 data from the embedded sample
        esp_err_t ret = audio_player_play_mp3_data(sample_mp3, sample_mp3_len);
        if (ret == ESP_OK) {
            strcpy(resp_str, "{\"status\": \"Started playing embedded MP3 sample\"}");
        } else {
            sprintf(resp_str, "{\"error\": \"Failed to start embedded MP3 playback: %s\"}", esp_err_to_name(ret));
        }
    }
    else {
        // Unknown command
        strcpy(resp_str, "{\"error\": \"Unknown command\"}");
    }
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

void start_rest_server(void) {
    httpd_handle_t server = NULL; // Declare and initialize the server handle
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t data_uri = {
        .uri       = "/api/data",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t play_uri = {
        .uri       = "/api/play",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t pause_uri = {
        .uri       = "/api/pause",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t volume_uri = {
        .uri       = "/api/volume/*",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };

    // New MP3 endpoints
    httpd_uri_t mp3_play_uri = {
        .uri       = "/api/mp3/play",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t mp3_stop_uri = {
        .uri       = "/api/mp3/stop",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t mp3_embedded_uri = {
        .uri       = "/api/mp3/play-embedded",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &data_uri);
        httpd_register_uri_handler(server, &play_uri);
        httpd_register_uri_handler(server, &pause_uri);
        httpd_register_uri_handler(server, &volume_uri);
        httpd_register_uri_handler(server, &mp3_play_uri);
        httpd_register_uri_handler(server, &mp3_stop_uri);
        httpd_register_uri_handler(server, &mp3_embedded_uri);
        ESP_LOGI(TAG, "HTTP server started");
    }
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize UART
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, 256, 0, 0, NULL, 0));
    
    ESP_LOGI(TAG, "UART initialized");

    // Initialize Wi-Fi
    wifi_init_softap();

    // Initialize Audio Player
    ESP_ERROR_CHECK(audio_player_init(I2S_PORT_NUM));
    ESP_LOGI(TAG, "Audio player initialized");

    // Start HTTP server
    start_rest_server();
}