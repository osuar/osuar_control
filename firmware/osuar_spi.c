#include <osuar_spi.h>

/*
 * Mutex to lock output buffer
 */
static Mutex spi_mtx; /* Mutex */


void setup_spi(void)
{
	palSetPadMode(GPIOB, 3, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* SPI1 SCK */
	palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* SPI1 MISO */
	palSetPadMode(GPIOB, 5, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* SPI1 MOSI */

	/*
	 * Initialize mutex
	 */
	chMtxInit(&spi_mtx); /* Initialize mutex. */
}

void spi_exchange(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *txbuf, uint8_t *rxbuf, uint16_t bufsize)
{
	chMtxLock(&spi_mtx);

	spiAcquireBus(spid);                        /* Acquire ownership of the bus. */
	spiStart(spid, spicfg);                     /* Set up transfer parameters. */

	spiSelect(spid);                            /* Assert slave select. */
	spiExchange(spid, bufsize, txbuf, rxbuf);   /* Atomic transfer operations. */
	spiUnselect(spid);                          /* Deassert slave select. */

	spiReleaseBus(spid);                        /* Release ownership. */

	chMtxUnlock();
}

void spi_send(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *txbuf, uint16_t bufsize)
{
	chMtxLock(&spi_mtx);

	spiAcquireBus(spid);                        /* Acquire ownership of the bus. */
	spiStart(spid, spicfg);                     /* Set up transfer parameters. */

	spiSelect(spid);                            /* Assert slave select. */
	spiSend(spid, bufsize, txbuf);              /* Send command. */
	spiUnselect(spid);                          /* Deassert slave select. */
	spiStop(spid);

	spiReleaseBus(spid);                        /* Release ownership. */

	chMtxUnlock();
}

void spi_receive(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *rxbuf, uint16_t bufsize)
{
	chMtxLock(&spi_mtx);

	spiAcquireBus(spid);                        /* Acquire ownership of the bus. */
	spiStart(spid, spicfg);                     /* Set up transfer parameters. */

	spiSelect(spid);                            /* Assert slave select. */
	spiReceive(spid, bufsize, rxbuf);           /* Receive data. */
	spiUnselect(spid);                          /* Deassert slave select. */

	spiReleaseBus(spid);                        /* Release ownership. */

	chMtxUnlock();
}


