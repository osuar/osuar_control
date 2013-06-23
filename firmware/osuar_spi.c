#include <osuar_spi.h>

void setup_spi(void)
{
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* SPI3 SCK */
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* SPI3 MISO */
	palSetPadMode(GPIOB, 12, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* SPI3 MOSI */

	/* TODO: May need a mutex when we begin using more than one SPI slave. */
}

void comm_spi(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *txbuf, uint8_t *rxbuf, uint16_t bufsize)
{
	spiAcquireBus(spid);                        /* Acquire ownership of the bus. */
	spiStart(spid, spicfg);                     /* Set up transfer parameters. */
	spiSelect(spid);                            /* Assert slave select. */
	spiExchange(spid, bufsize, txbuf, rxbuf);   /* Atomic transfer operations. */
	spiUnselect(spid);                          /* Deassert slave select. */
	spiReleaseBus(spid);                        /* Release ownership. */
}

