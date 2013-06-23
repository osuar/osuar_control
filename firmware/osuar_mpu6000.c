#include <osuar_mpu6000.h>

static uint8_t mpu_txbuf[MPU_SPI_BUFSIZE];
static uint8_t mpu_rxbuf[MPU_SPI_BUFSIZE];

/**
 * @brief Maximum speed SPI configuration for MPU-6000 (656.250 kHz, CPHA=0,
 * CPOL=0, MSb first).
 */
static const SPIConfig mpu_spicfg = {
	NULL,
	GPIOB,
	2,
	SPI_CR1_BR_2 | SPI_CR1_BR_0   /* 21000000/2^5 = 656250 */
};


void setup_mpu(void)
{
	palSetPadMode(GPIOB, 2, PAL_MODE_OUTPUT_PUSHPULL);   /* MPU-6000 CS */
	palSetPad(GPIOB, 2);   /* Unselect */
}

void read_mpu(float gyr[3], float acc[3])
{
	comm_spi(&SPID3, &mpu_spicfg, mpu_txbuf, mpu_rxbuf, MPU_SPI_BUFSIZE);

	int i;
	for (i=0; i<3; i++) {
		gyr[i] = mpu_rxbuf[8 + i*2];
		acc[i] = mpu_rxbuf[0 + i*2];
	}

	uart_printf("hello\n");
}

