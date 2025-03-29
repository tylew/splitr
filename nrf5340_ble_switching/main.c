#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/capabilities.h>
#include <zephyr/zbus/zbus.h>

#include "audio_system.h"
#include "bt_mgmt.h"
#include "bt_content_ctrl.h"
#include "nrf5340_audio_common.h"
#include "audio_datapath.h"
#include "le_audio_rx.h"
#include "button_handler.h"
#include "macros_common.h"
#include "src/uart_handler.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

/* Audio configuration */
#define AUDIO_FRAME_DURATION_US 10000  // 10ms frame duration
#define AUDIO_SAMPLE_RATE_HZ    48000  // 48kHz sample rate
#define AUDIO_BITRATE          128000  // 128 kbps

static enum {
    MODE_BROADCAST,
    MODE_SINK
} current_mode = MODE_BROADCAST;

/* Data FIFO for audio */
static struct data_fifo audio_fifo_rx;

ZBUS_SUBSCRIBER_DEFINE(button_sub, CONFIG_BUTTON_MSG_SUB_QUEUE_SIZE);

static void handle_mode_switch(void)
{
    int ret;
    
    LOG_INF("Mode switch button pressed");
    
    // Stop current audio and BLE operations
    audio_system_stop();
    audio_datapath_stop();
    bt_mgmt_disconnect();

    // Switch mode
    if (current_mode == MODE_BROADCAST) {
        LOG_INF("Switching to SINK mode");
        current_mode = MODE_SINK;
        
        // Initialize audio RX path
        ret = le_audio_rx_init();
        if (ret) {
            LOG_ERR("Failed to init audio RX: %d", ret);
            return;
        }

        // Start scanning for broadcast
        ret = bt_mgmt_scan_start(SCAN_TYPE_BROADCAST);
        if (ret) {
            LOG_ERR("Failed to start scanning: %d", ret);
            return;
        }

    } else {
        LOG_INF("Switching to BROADCAST mode");
        current_mode = MODE_BROADCAST;
        
        // Configure audio system for broadcasting
        ret = audio_system_config_set(AUDIO_SAMPLE_RATE_HZ, 
                                    AUDIO_BITRATE,
                                    AUDIO_SAMPLE_RATE_HZ);
        if (ret) {
            LOG_ERR("Failed to configure audio: %d", ret);
            return;
        }

        // Start audio system
        audio_system_start();
        
        // Start broadcasting
        ret = bt_mgmt_adv_start(ADV_TYPE_BROADCAST);
        if (ret) {
            LOG_ERR("Failed to start broadcasting: %d", ret);
            return;
        }
    }
}

/* Button message handler thread */
static void button_msg_handler(void)
{
    int ret;
    struct button_msg msg;

    while (1) {
        ret = zbus_sub_wait(&button_sub, &msg, K_FOREVER);
        ERR_CHK(ret);

        switch (msg.button_pin) {
        case BUTTON_PLAY_PAUSE:
            if (msg.button_action == BUTTON_PRESS) {
                handle_mode_switch();
            }
            break;

        default:
            break;
        }
    }
}

K_THREAD_DEFINE(button_msg_thread_id, CONFIG_BUTTON_MSG_SUB_STACK_SIZE, button_msg_handler, NULL,
                NULL, NULL, K_PRIO_PREEMPT(CONFIG_BUTTON_MSG_SUB_THREAD_PRIO), 0, 0);

/* Callback for when broadcast audio is received */
static void broadcast_started_cb(void)
{
    int ret;

    if (current_mode != MODE_SINK) {
        return;
    }

    LOG_INF("Broadcast received, starting audio datapath");

    // Start audio datapath for receiving
    ret = audio_datapath_start(&audio_fifo_rx);
    if (ret) {
        LOG_ERR("Failed to start audio datapath: %d", ret);
        return;
    }
}

int main(void)
{
    int ret;

    LOG_INF("nRF5340 BLE Mode Switching Application");

    /* Initialize audio datapath */
    ret = audio_datapath_init();
    if (ret) {
        LOG_ERR("Failed to initialize audio datapath: %d", ret);
        return ret;
    }

    /* Initialize audio system */
    ret = audio_system_init();
    if (ret) {
        LOG_ERR("Failed to initialize audio system: %d", ret);
        return ret;
    }

    /* Initialize BLE management */
    ret = bt_mgmt_init();
    if (ret) {
        LOG_ERR("Failed to initialize BT management: %d", ret);
        return ret;
    }

    /* Initialize content control */
    ret = bt_content_ctrl_init();
    if (ret) {
        LOG_ERR("Failed to initialize content control: %d", ret);
        return ret;
    }

    /* Initialize button handling */
    ret = button_handler_init();
    if (ret) {
        LOG_ERR("Failed to initialize button handler: %d", ret);
        return ret;
    }

    /* Initialize UART handler */
    ret = uart_handler_init();
    if (ret) {
        LOG_ERR("Failed to initialize UART handler: %d", ret);
        return ret;
    }

    /* Configure initial audio settings */
    ret = audio_system_config_set(AUDIO_SAMPLE_RATE_HZ, 
                                 AUDIO_BITRATE,
                                 AUDIO_SAMPLE_RATE_HZ);
    if (ret) {
        LOG_ERR("Failed to configure audio: %d", ret);
        return ret;
    }

    /* Start audio system */
    audio_system_start();

    /* Start in broadcast mode by default */
    ret = bt_mgmt_adv_start(ADV_TYPE_BROADCAST);
    if (ret) {
        LOG_ERR("Failed to start broadcasting: %d", ret);
        return ret;
    }

    LOG_INF("Application started in BROADCAST mode");
    LOG_INF("Press PLAY/PAUSE button to switch modes");
    LOG_INF("UART communication enabled (115200 baud, RX on pin 16)");

    return 0;
}