/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "streamctrl.h"

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#include "nrf5340_audio_common.h"
#include "nrf5340_audio_dk.h"
#include "broadcast_source.h"
#include "led.h"
#include "button_assignments.h"
#include "macros_common.h"
#include "audio_system.h"
#include "bt_mgmt.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

/*
 * Stereo broadcast: define two streams and map them into left/right subgroups
 */
static struct bt_bap_stream streams[2];
static struct bt_bap_broadcast_source *broadcast_source;

/* External codec / QoS configs from LC3 and ISO */
extern const struct bt_bap_lc3_config      lc3_config;
extern const struct bt_iso_chan_qos        data_qos;
extern const struct bt_iso_chan_qos        meta_qos;

/* Group each stream into its own subgroup: [0]=left, [1]=right */
static struct bt_bap_stream *left_stream_group[]  = { &streams[0] };
static struct bt_bap_stream *right_stream_group[] = { &streams[1] };

static struct bt_bap_broadcast_source_subgroup subgroups[] = {
    BT_BAP_BROADCAST_SOURCE_SUBGROUP(
        left_stream_group,
        ARRAY_SIZE(left_stream_group),
        &lc3_config,
        &data_qos,
        &meta_qos),
    BT_BAP_BROADCAST_SOURCE_SUBGROUP(
        right_stream_group,
        ARRAY_SIZE(right_stream_group),
        &lc3_config,
        &data_qos,
        &meta_qos),
};

static struct bt_bap_broadcast_source_create_param create_param = {
    .subgroups      = subgroups,
    .subgroup_count = ARRAY_SIZE(subgroups),
};

ZBUS_SUBSCRIBER_DEFINE(button_evt_sub, CONFIG_BUTTON_MSG_SUB_QUEUE_SIZE);

ZBUS_MSG_SUBSCRIBER_DEFINE(le_audio_evt_sub);

ZBUS_CHAN_DECLARE(button_chan);
ZBUS_CHAN_DECLARE(le_audio_chan);
ZBUS_CHAN_DECLARE(bt_mgmt_chan);
ZBUS_CHAN_DECLARE(sdu_ref_chan);

ZBUS_OBS_DECLARE(sdu_ref_msg_listen);

static struct k_thread button_msg_sub_thread_data;
static struct k_thread le_audio_msg_sub_thread_data;

static k_tid_t button_msg_sub_thread_id;
static k_tid_t le_audio_msg_sub_thread_id;

K_THREAD_STACK_DEFINE(button_msg_sub_thread_stack, CONFIG_BUTTON_MSG_SUB_STACK_SIZE);
K_THREAD_STACK_DEFINE(le_audio_msg_sub_thread_stack, CONFIG_LE_AUDIO_MSG_SUB_STACK_SIZE);

static enum stream_state strm_state = STATE_PAUSED;

/* Function for handling all stream state changes */
static void stream_state_set(enum stream_state stream_state_new)
{
    strm_state = stream_state_new;
}

/**
 * @brief   Handle button activity.
 */
static void button_msg_sub_thread(void)
{
    int ret;
    const struct zbus_channel *chan;

    while (1) {
        ret = zbus_sub_wait(&button_evt_sub, &chan, K_FOREVER);
        ERR_CHK(ret);

        struct button_msg msg;

        ret = zbus_chan_read(chan, &msg, ZBUS_READ_TIMEOUT_MS);
        ERR_CHK(ret);

        LOG_DBG("Got btn evt from queue - id = %d, action = %d", msg.button_pin,
                msg.button_action);

        if (msg.button_action != BUTTON_PRESS) {
            LOG_WRN("Unhandled button action");
            return;
        }

        switch (msg.button_pin) {
        case BUTTON_PLAY_PAUSE:
            if (strm_state == STATE_STREAMING) {
                ret = broadcast_source_stop(broadcast_source);
                if (ret) {
                    LOG_WRN("Failed to stop broadcaster: %d", ret);
                }
            } else if (strm_state == STATE_PAUSED) {
                ret = broadcast_source_start(broadcast_source, NULL);
                if (ret) {
                    LOG_WRN("Failed to start broadcaster: %d", ret);
                }
            } else {
                LOG_WRN("In invalid state: %d", strm_state);
            }
            break;

        case BUTTON_4:
            if (IS_ENABLED(CONFIG_AUDIO_TEST_TONE)) {
                if (strm_state != STATE_STREAMING) {
                    LOG_WRN("Not in streaming state");
                    break;
                }

                ret = audio_system_encode_test_tone_step();
                if (ret) {
                    LOG_WRN("Failed to play test tone, ret: %d", ret);
                }
                break;
            }
            break;

        default:
            LOG_WRN("Unexpected/unhandled button id: %d", msg.button_pin);
        }

        STACK_USAGE_PRINT("button_msg_thread", &button_msg_sub_thread_data);
    }
}

/**
 * @brief   Handle Bluetooth LE audio events.
 */
static void le_audio_msg_sub_thread(void)
{
    int ret;
    const struct zbus_channel *chan;

    while (1) {
        struct le_audio_msg msg;

        ret = zbus_sub_wait_msg(&le_audio_evt_sub, &chan, &msg, K_FOREVER);
        ERR_CHK(ret);

        LOG_DBG("Received event = %d, current state = %d", msg.event, strm_state);

        switch (msg.event) {
        case LE_AUDIO_EVT_STREAMING:
            LOG_DBG("LE audio evt streaming");

            audio_system_encoder_start();

            if (strm_state == STATE_STREAMING) {
                LOG_DBG("Got streaming event in streaming state");
                break;
            }

            audio_system_start();
            stream_state_set(STATE_STREAMING);
            ret = led_blink(LED_APP_1_BLUE);
            ERR_CHK(ret);
            break;

        case LE_AUDIO_EVT_NOT_STREAMING:
            LOG_DBG("LE audio evt not_streaming");

            audio_system_encoder_stop();

            if (strm_state == STATE_PAUSED) {
                LOG_DBG("Got not_streaming event in paused state");
                break;
            }

            stream_state_set(STATE_PAUSED);
            audio_system_stop();
            ret = led_on(LED_APP_1_BLUE);
            ERR_CHK(ret);
            break;

        default:
            LOG_WRN("Unexpected/unhandled le_audio event: %d", msg.event);
            break;
        }

        STACK_USAGE_PRINT("le_audio_msg_thread", &le_audio_msg_sub_thread_data);
    }
}

/**
 * @brief   Create zbus subscriber threads.
 *
 * @return  0 for success, error otherwise.
 */
static int zbus_subscribers_create(void)
{
    int ret;

    button_msg_sub_thread_id = k_thread_create(
        &button_msg_sub_thread_data, button_msg_sub_thread_stack,
        CONFIG_BUTTON_MSG_SUB_STACK_SIZE, (k_thread_entry_t)button_msg_sub_thread, NULL,
        NULL, NULL, K_PRIO_PREEMPT(CONFIG_BUTTON_MSG_SUB_THREAD_PRIO), 0, K_NO_WAIT);
    ret = k_thread_name_set(button_msg_sub_thread_id, "BUTTON_MSG_SUB");
    if (ret) {
        LOG_ERR("Failed to create button_msg thread");
        return ret;
    }

    le_audio_msg_sub_thread_id = k_thread_create(
        &le_audio_msg_sub_thread_data, le_audio_msg_sub_thread_stack,
        CONFIG_LE_AUDIO_MSG_SUB_STACK_SIZE, (k_thread_entry_t)le_audio_msg_sub_thread, NULL,
        NULL, NULL, K_PRIO_PREEMPT(CONFIG_LE_AUDIO_MSG_SUB_THREAD_PRIO), 0, K_NO_WAIT);
    ret = k_thread_name_set(le_audio_msg_sub_thread_id, "LE_AUDIO_MSG_SUB");
    if (ret) {
        LOG_ERR("Failed to create le_audio_msg thread");
        return ret;
    }

    ret = zbus_chan_add_obs(&sdu_ref_chan, &sdu_ref_msg_listen, ZBUS_ADD_OBS_TIMEOUT_MS);
    if (ret) {
        LOG_ERR("Failed to add timestamp listener");
        return ret;
    }

    return 0;
}

/**
 * @brief   Zbus listener to receive events from bt_mgmt.
 */
static void bt_mgmt_evt_handler(const struct zbus_channel *chan)
{
    int ret;
    const struct bt_mgmt_msg *msg;

    msg = zbus_chan_const_msg(chan);

    switch (msg->event) {
    case BT_MGMT_EXT_ADV_WITH_PA_READY:
        LOG_INF("Ext adv ready");

        /* Create broadcast source with stereo grouping */
        ret = bt_bap_broadcast_source_create(&broadcast_source, &create_param);
        if (ret) {
            LOG_ERR("Failed to create broadcast source: %d", ret);
            return;
        }

        /* Start broadcasting */
        ret = broadcast_source_start(broadcast_source, msg->ext_adv);
        if (ret) {
            LOG_ERR("Failed to start broadcaster: %d", ret);
        }
        break;

    default:
        LOG_WRN("Unexpected/unhandled bt_mgmt event: %d", msg->event);
        break;
    }
}

ZBUS_LISTENER_DEFINE(bt_mgmt_evt_listen, bt_mgmt_evt_handler);

/**
 * @brief   Link zbus producers and observers.
 *
 * @return  0 for success, error otherwise.
 */
static int zbus_link_producers_observers(void)
{
    int ret;

    if (!IS_ENABLED(CONFIG_ZBUS)) {
        return -ENOTSUP;
    }

    ret = zbus_chan_add_obs(&button_chan, &button_evt_sub, ZBUS_ADD_OBS_TIMEOUT_MS);
    if (ret) {
        LOG_ERR("Failed to add button sub");
        return ret;
    }

    ret = zbus_chan_add_obs(&le_audio_chan, &le_audio_evt_sub, ZBUS_ADD_OBS_TIMEOUT)

}