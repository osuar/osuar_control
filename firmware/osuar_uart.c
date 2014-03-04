#include <ch.h>
#include <hal.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <osuar_comm.h>

/*
 * UART printf buffer
 */
#define PRINTF_BUF_SIZE 80
static char uart_print_buf[PRINTF_BUF_SIZE];
static Semaphore uart_sem;

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp)
{
	(void)uartp;

	chSysLockFromIsr();
	chSemSignalI(&uart_sem);
	chSysUnlockFromIsr();
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp)
{
	(void)uartp;
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e)
{
	(void)uartp;
	(void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c)
{
	(void)uartp;
	(void)c;
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(UARTDriver *uartp)
{
	(void)uartp;
}

/*
 * USART1 configuration structure.
 */
static UARTConfig uart1cfg = {
	txend1,
	txend2,
	rxend,
	rxchar,
	rxerr,
	460800,
	0,
	USART_CR2_LINEN,
	0
};

/*
 * USART3 configuration structure.
 */
static UARTConfig uart3cfg = {
	txend1,
	txend2,
	osuar_comm_data_received,
	rxchar,
	rxerr,
	460800,
	0,
	USART_CR2_LINEN,
	0
};


void setup_uart(void)
{
	/*
	 * Initialize the Semaphore
	 */
	chSemInit(&uart_sem, 1);

	/*
	 * Activate USART1 and USART3
	 * TODO: I would enable USART2, as well, but for some reason it doesn't
	 * work when I2C1 is enabled.
	 */
	uartStart(&UARTD1, &uart1cfg);
	uartStart(&UARTD3, &uart3cfg);

	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(7));   // USART1 TX
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(7));   // USART1 RX
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));   // USART3 TX
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));   // USART3 RX
}

void uart_printf(const char *format, ...)
{
	va_list ap; 
	va_start (ap, format);

	chSemWait(&uart_sem);

	/* 
	 * Build string to send to the buffer.
	 */
	vsnprintf(uart_print_buf, PRINTF_BUF_SIZE, format, ap);

	/*
	 * Print stuff UART
	 */
	uartStartSend(&UARTD3, strlen(uart_print_buf), uart_print_buf);
}

void uart_printf_i(const char *format, ...)
{
	va_list ap; 
	va_start (ap, format);
	
	chSemWait(&uart_sem);

	/*
	 * Build string to send to the buffer.
	 */
	vsnprintf(uart_print_buf, PRINTF_BUF_SIZE, format, ap);

	/*
	 * Print stuff UART
	 */
	uartStartSendI(&UARTD3, strlen(uart_print_buf), uart_print_buf);
}

void clear_buffer(uint8_t *buffer)
{
	uint16_t i;
	for (i=0; i<sizeof(buffer); i++) {
		buffer[i] = 0;
	}
}

