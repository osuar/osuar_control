#include <osuar_comm.h>

void setup_comm(void)
{
	/*
	 * Activate USART2 and USART3
	 */
	uartStart(&UARTD2, &uart2cfg);
	uartStart(&UARTD3, &uart3cfg);
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));   // USART2 TX
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));   // USART2 RX
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));   // USART3 TX
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));   // USART3 RX

}

