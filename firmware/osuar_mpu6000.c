#include <osuar_mpu6000.h>

static uint8_t mpu_txbuf[MPU_SPI_BUFSIZE];
static uint8_t mpu_rxbuf[MPU_SPI_BUFSIZE];
static float dbg_gyr[3], dbg_acc[3];

/**
 * @brief High speed SPI configuration for MPU-6000 (10.5 MHz, CPHA=0,
 * CPOL=0, MSb first). The MPU-6000 supports SPI reads of sensor and interrupt
 * registers at up to 20 MHz.
 */
static const SPIConfig mpu_hs_spicfg = {
	NULL,
	GPIOB,
	2,
	SPI_CR1_BR_1   /* 42000000/2^2 = 10500000 */
};

/**
 * @brief Low speed SPI configuration for MPU-6000 (656.250 kHz, CPHA=0,
 * CPOL=0, MSb first). The MPU-6000 supports SPI communication with all
 * registers at up to 1 MHz.
 */
static const SPIConfig mpu_ls_spicfg = {
	NULL,
	GPIOB,
	2,
	SPI_CR1_BR_2 | SPI_CR1_BR_1   /* 42000000/2^6 = 656250 */
};

/**
 * @brief Temperature.
 */
float mpu_temp;


void setup_mpu(void)
{
	palSetPadMode(GPIOB, 2, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);   /* MPU-6000 CS */
	palSetPad(GPIOB, 2);   /* Unselect */

	/* Reset device. */
	mpu_txbuf[0] = MPU6000_PWR_MGMT_1;
	mpu_txbuf[1] = 0x80;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	chThdSleepMilliseconds(100);

	/* Wake up device and set clock source to Gyro Z. */
	mpu_txbuf[0] = MPU6000_PWR_MGMT_1;
	mpu_txbuf[1] = 0x03;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	/* Set sample rate to 1 kHz. */
	mpu_txbuf[0] = MPU6000_SMPLRT_DIV;
	mpu_txbuf[1] = 0x00;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	/* Set gyro full range to 500 dps. We first read in the current register
	 * value so we can change what we need and leave everything else alone. */
	mpu_txbuf[0] = MPU6000_GYRO_CONFIG | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */
	mpu_txbuf[0] = MPU6000_GYRO_CONFIG;
	mpu_txbuf[1] = (mpu_rxbuf[1] & ~0x18) | 0x08;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	/* Set accelerometer full range to 2 g. */
	mpu_txbuf[0] = MPU6000_ACCEL_CONFIG | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */
	mpu_txbuf[0] = MPU6000_ACCEL_CONFIG;
	mpu_txbuf[1] = (mpu_rxbuf[1] & ~0x18) | 0x00;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */
}

void read_mpu(float gyr[3], float acc[3])
{
	/* Read gyroscope. */
	mpu_txbuf[0] = MPU6000_GYRO_XOUT_H | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 7);
	gyr[0] = ((int16_t) -((mpu_rxbuf[1]<<8) | mpu_rxbuf[2])) / 65.5 * 3.14159 / 180 + GYR_X_OFFSET;
	gyr[1] = ((int16_t) -((mpu_rxbuf[3]<<8) | mpu_rxbuf[4])) / 65.5 * 3.14159 / 180 + GYR_Y_OFFSET;
	gyr[2] = ((int16_t) -((mpu_rxbuf[5]<<8) | mpu_rxbuf[6])) / 65.5 * 3.14159 / 180 + GYR_Z_OFFSET;

	/* Read accelerometer. */
	mpu_txbuf[0] = MPU6000_ACCEL_XOUT_H | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 7);
	acc[0] = ((int16_t) -((mpu_rxbuf[1]<<8) | mpu_rxbuf[2])) / 16384.0 + ACC_X_OFFSET;
	acc[1] = ((int16_t) -((mpu_rxbuf[3]<<8) | mpu_rxbuf[4])) / 16384.0 + ACC_Y_OFFSET;
	acc[2] = ((int16_t) -((mpu_rxbuf[5]<<8) | mpu_rxbuf[6])) / 16384.0 + ACC_Z_OFFSET;

	/* Read temperature. */
	mpu_txbuf[0] = MPU6000_TEMP_OUT_H | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 3);
	mpu_temp = ((int16_t) ((mpu_rxbuf[1]<<8) | mpu_rxbuf[2])) / 340 + 36.53;

	/* Copy sensor reads to debug buffers. */
	uint8_t i;
	for (i=0; i<3; i++) {
		dbg_gyr[i] = gyr[i];
		dbg_acc[i] = acc[i];
	}
}

void debug_mpu(uint8_t *buffer)
{
	static uint16_t count = 1;
	static float g0 = 0;
	static float g1 = 0;
	static float g2 = 0;
	g0 = (g0*count + dbg_acc[0]) / (count+1);
	g1 = (g1*count + dbg_acc[1]) / (count+1);
	g2 = (g2*count + dbg_acc[2]) / (count+1);

	count += 1;

	chsprintf(buffer, "%8u %7d %7d %7d\r\n",
			count,
			(int32_t) (dbg_acc[0]*1000000),
			(int32_t) (dbg_acc[1]*1000000),
			(int32_t) (dbg_acc[2]*1000000));
}

