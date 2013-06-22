#ifndef OSUAR_SPI_H
#define OSUAR_SPI_H

#include <ch.h>
#include <hal.h>

/**
 * SPI I/O pins setup.
 */
void setup_spi(void);

/**
 * SPI communication.
 */
void comm_spi(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *txbuf, uint8_t *rxbuf, uint16_t bufsize);

#endif /* OSUAR_SPI_H */

