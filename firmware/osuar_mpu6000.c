#include <osuar_mpu6000.h>

static uint8_t mpu_txbuf[MPU_SPI_BUFSIZE];
static uint8_t mpu_rxbuf[MPU_SPI_BUFSIZE];

void mpu_read(float gyro[3], float accel[3])
{
	comm_spi(&SPID3, &mpu_spicfg, mpu_txbuf, mpu_rxbuf, MPU_SPI_BUFSIZE);

	int i;
	for (i=0; i<3; i++) {
		gyro[i] = 0.3;
		accel[i] = 0.5;
	}
}

