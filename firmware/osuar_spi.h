#ifndef OSUAR_SPI_H
#define OSUAR_SPI_H

#include <ch.h>
#include <hal.h>

/**
 * SPI I/O pins setup.
 */
void setup_spi(void);

/**
 * @brief SPI exchange
 */
void spi_exchange(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *txbuf, uint8_t *rxbuf, uint16_t bufsize);

/**
 * @brief SPI send
 */
void spi_send(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *txbuf, uint16_t bufsize);

/**
 * @brief SPI receive
 */
void spi_receive(SPIDriver *spid, const SPIConfig *spicfg, uint8_t *rxbuf, uint16_t bufsize);

#endif /* OSUAR_SPI_H */

