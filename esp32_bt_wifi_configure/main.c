#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_spp_api.h"
#include "string.h"

#define BT_DEVICE_NAME "ESP32_Config"
#define BT_PASSWORD "1234"
#define MAX_WIFI_NETWORKS 10
#define WIFI_SCAN_TIME_MS 3000

static const char *TAG = "ESP_BT_WIFI";
static bool authenticated = false;
static uint32_t spp_client_handle = 0;

// Wi-Fi scan function
void scan_wifi_networks() {
    uint16_t num_networks = MAX_WIFI_NETWORKS;
    wifi_ap_record_t ap_records[MAX_WIFI_NETWORKS];
    
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };
    
    esp_wifi_scan_start(&scan_config, true);
    esp_wifi_scan_get_ap_records(&num_networks, ap_records);
    
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "Found %d networks:\n", num_networks);
    
    for (int i = 0; i < num_networks; i++) {
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%d. %s (RSSI: %d)\n", i + 1, ap_records[i].ssid, ap_records[i].rssi);
    }
    
    if (spp_client_handle) {
        esp_spp_write(spp_client_handle, strlen(buffer), (uint8_t *)buffer);
    }
}

// Bluetooth SPP event handler
void bt_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_SRV_OPEN_EVT:
            ESP_LOGI(TAG, "Client Connected");
            spp_client_handle = param->srv_open.handle;
            break;
        case ESP_SPP_DATA_IND_EVT:
            if (param->data_ind.len > 0) {
                char received[100];
                memcpy(received, param->data_ind.data, param->data_ind.len);
                received[param->data_ind.len] = '\0';
                ESP_LOGI(TAG, "Received: %s", received);
                
                if (!authenticated) {
                    if (strcmp(received, BT_PASSWORD) == 0) {
                        authenticated = true;
                        esp_spp_write(spp_client_handle, 9, (uint8_t *)"AUTH_OK\n");
                    } else {
                        esp_spp_write(spp_client_handle, 11, (uint8_t *)"AUTH_FAIL\n");
                    }
                } else if (strcmp(received, "SCAN") == 0) {
                    scan_wifi_networks();
                } else if (strncmp(received, "CONNECT", 7) == 0) {
                    char ssid[32], password[64];
                    sscanf(received + 8, "%31s %63s", ssid, password);
                    
                    wifi_config_t wifi_config = {};
                    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
                    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
                    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
                    esp_wifi_connect();
                    esp_spp_write(spp_client_handle, 13, (uint8_t *)"WIFI_CONNECT\n");
                }
            }
            break;
        default:
            break;
    }
}

// Initialize Bluetooth SPP
void init_bluetooth() {
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    
    esp_spp_register_callback(bt_spp_cb);
    esp_spp_init(ESP_SPP_MODE_CB);
    
    esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATED, ESP_SPP_ROLE_SLAVE, 0, BT_DEVICE_NAME);
}

// Initialize Wi-Fi in station mode
void init_wifi() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
}

void app_main(void) {
    nvs_flash_init();
    init_wifi();
    init_bluetooth();
}
