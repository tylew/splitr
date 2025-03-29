#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/logging/log.h>

#include "audio_system.h"
#include "audio_datapath.h"
#include "bt_mgmt.h"
#include "bt_content_ctrl.h"

LOG_MODULE_REGISTER(uart_handler, CONFIG_LOG_DEFAULT_LEVEL);

#define UART_DEVICE_NODE DT_NODELABEL(uart0)
#define UART_BUF_SIZE 256

static const struct device *uart_dev;
static char rx_buf[UART_BUF_SIZE];
static int rx_buf_pos;

// External function declarations for audio control
extern void audio_system_start(void);
extern void audio_system_stop(void);
extern int audio_system_volume_set(uint8_t volume);

// UART callback
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    switch (evt->type) {
        case UART_RX_RDY:
            if ((rx_buf_pos + evt->data.rx.len) < UART_BUF_SIZE) {
                memcpy(&rx_buf[rx_buf_pos], &evt->data.rx.buf[evt->data.rx.offset], evt->data.rx.len);
                rx_buf_pos += evt->data.rx.len;
                
                // Check if we've received a newline character
                for (int i = 0; i < rx_buf_pos; i++) {
                    if (rx_buf[i] == '\n') {
                        // Null-terminate the string
                        rx_buf[i] = '\0';
                        
                        // Process the command
                        if (strcmp(rx_buf, "PLAY") == 0) {
                            LOG_INF("Received PLAY command");
                            audio_system_start();
                        } 
                        else if (strcmp(rx_buf, "PAUSE") == 0) {
                            LOG_INF("Received PAUSE command");
                            audio_system_stop();
                        }
                        else if (strncmp(rx_buf, "VOL:", 4) == 0) {
                            int volume = atoi(&rx_buf[4]);
                            LOG_INF("Received VOLUME command: %d", volume);
                            if (volume >= 0 && volume <= 100) {
                                audio_system_volume_set(volume);
                            } else {
                                LOG_WRN("Invalid volume level: %d", volume);
                            }
                        }
                        else {
                            LOG_WRN("Unknown command: %s", rx_buf);
                        }
                        
                        // Move any remaining data to the beginning of the buffer
                        if (i < rx_buf_pos - 1) {
                            memmove(rx_buf, &rx_buf[i + 1], rx_buf_pos - i - 1);
                            rx_buf_pos = rx_buf_pos - i - 1;
                        } else {
                            rx_buf_pos = 0;
                        }
                        break;
                    }
                }
            } else {
                LOG_ERR("UART buffer overflow");
                rx_buf_pos = 0;
            }
            break;
            
        case UART_RX_BUF_REQUEST:
            // Provide a new buffer for RX
            uart_rx_buf_rsp(dev, rx_buf, sizeof(rx_buf));
            break;
            
        case UART_RX_DISABLED:
            // Re-enable RX
            uart_rx_enable(dev, rx_buf, sizeof(rx_buf), 100);
            break;
            
        default:
            break;
    }
}

int uart_handler_init(void)
{
    int ret;
    
    uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
    if (!device_is_ready(uart_dev)) {
        LOG_ERR("UART device not ready");
        return -ENODEV;
    }
    
    rx_buf_pos = 0;
    
    ret = uart_callback_set(uart_dev, uart_cb, NULL);
    if (ret) {
        LOG_ERR("Failed to set UART callback: %d", ret);
        return ret;
    }
    
    ret = uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), 100);
    if (ret) {
        LOG_ERR("Failed to enable UART RX: %d", ret);
        return ret;
    }
    
    LOG_INF("UART handler initialized");
    return 0;
} 