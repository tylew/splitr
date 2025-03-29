#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "string.h"

#define EXAMPLE_ESP_WIFI_SSID      "ESP32-Access-Point"
#define EXAMPLE_ESP_WIFI_PASS      "123456789"
#define EXAMPLE_MAX_STA_CONN       4

#define UART_NUM UART_NUM_1
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define BUF_SIZE (1024)

// === UART Setup ===
void init_uart() {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void send_uart_command(const char* command) {
    uart_write_bytes(UART_NUM, command, strlen(command));
}

// === Wi-Fi AP Setup ===
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

// === REST Handlers ===

static esp_err_t data_get_handler(httpd_req_t *req) {
    const char* resp_str = "{\"message\": \"Hello, World!\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

static esp_err_t play_handler(httpd_req_t *req) {
    send_uart_command("PLAY\n");
    httpd_resp_sendstr(req, "OK: PLAY");
    return ESP_OK;
}

static esp_err_t pause_handler(httpd_req_t *req) {
    send_uart_command("PAUSE\n");
    httpd_resp_sendstr(req, "OK: PAUSE");
    return ESP_OK;
}

static esp_err_t volume_handler(httpd_req_t *req) {
    char volume_str[8];
    // Extract volume from URI: /volume/70
    const char *uri = req->uri;
    const char *vol_ptr = strrchr(uri, '/');
    if (vol_ptr && strlen(vol_ptr + 1) < sizeof(volume_str)) {
        strcpy(volume_str, vol_ptr + 1);
        int volume = atoi(volume_str);
        char vol_cmd[16];
        snprintf(vol_cmd, sizeof(vol_cmd), "VOL:%d\n", volume);
        send_uart_command(vol_cmd);
        httpd_resp_sendstr(req, "OK: Volume set");
        return ESP_OK;
    }

    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid volume");
    return ESP_FAIL;
}

// === REST Server Setup ===
void start_rest_server(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t data_uri = {
        .uri       = "/api/data",
        .method    = HTTP_GET,
        .handler   = data_get_handler,
        .user_ctx  = NULL
    };
    httpd_uri_t play_uri = {
        .uri       = "/play",
        .method    = HTTP_GET,
        .handler   = play_handler,
        .user_ctx  = NULL
    };
    httpd_uri_t pause_uri = {
        .uri       = "/pause",
        .method    = HTTP_GET,
        .handler   = pause_handler,
        .user_ctx  = NULL
    };
    httpd_uri_t volume_uri = {
        .uri       = "/volume/*",
        .method    = HTTP_GET,
        .handler   = volume_handler,
        .user_ctx  = NULL
    };

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &data_uri);
        httpd_register_uri_handler(server, &play_uri);
        httpd_register_uri_handler(server, &pause_uri);
        httpd_register_uri_handler(server, &volume_uri);
    }
}

// === Main Entry ===
void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    init_uart();
    wifi_init_softap();
    start_rest_server();
}
