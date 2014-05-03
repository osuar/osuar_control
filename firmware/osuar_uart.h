#ifndef OSUAR_UART_H
#define OSUAR_UART_H

#include <ch.h>

/*
 * Initialize things.
 */
void setup_uart(void);

/*
 * Print formatted string to USART3.
 */
void uart_printf(const char *format, ...);

/*
 * Print formatted string to USART3 from interrupt.
 */
void uart_printf_i(const char *format, ...);

/*
 * Utility function to clear (i.e., zero) a buffer.
 */
void clear_buffer(uint8_t *buffer);

#endif /* OSUAR_UART_H */

