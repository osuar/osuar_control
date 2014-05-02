#ifndef OSUAR_UART_H
#define OSUAR_UART_H

#include <osuar_config.h>
#include <osuar_comm.h>   /* TODO(yoos): This shouldn't be here. osuar_comm should use osuar_uart as a driver, not the other way around. */

#include <ch.h>
#include <hal.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

