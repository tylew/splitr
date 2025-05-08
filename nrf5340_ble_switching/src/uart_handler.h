/**
 * @file uart_handler.h
 * @brief UART handler for receiving commands from ESP32
 */

#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the UART handler
 *
 * @return 0 if successful, negative error code otherwise
 */
int uart_handler_init(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_HANDLER_H */ 