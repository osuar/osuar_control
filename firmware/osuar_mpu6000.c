#include <osuar_mpu6000.h>
#include <osuar_config.h>
#include <osuar_math.h>
#include <osuar_spi.h>

#include <hal.h>
#include <chsprintf.h>

static uint8_t mpu_txbuf[MPU_SPI_BUFSIZE];
static uint8_t mpu_rxbuf[MPU_SPI_BUFSIZE];
static float dbg_gyr[3], dbg_acc[3], dbg_temp;

/**
 * @brief High speed SPI configuration for MPU-6000 (10.5 MHz, CPHA=0,
 * CPOL=0, MSb first). The MPU-6000 supports SPI reads of sensor and interrupt
 * registers at up to 20 MHz.
 */
static const SPIConfig mpu_hs_spicfg = {
	NULL,
	GPIOB,
	2,
	SPI_CR1_BR_0   /* 42000000/2^1 = 21000000 */
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

	/* Set sample rate to 1 kHz. */
	mpu_txbuf[0] = MPU6000_SMPLRT_DIV;
	mpu_txbuf[1] = 0x00;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	/* Set DLPF to 4 (20 Hz gyro bandwidth, 21 Hz accelerometer bandwidth) */
	mpu_txbuf[0] = MPU6000_CONFIG;
	mpu_txbuf[1] = 0x04;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	/* Wake up device and set clock source to Gyro Z. */
	mpu_txbuf[0] = MPU6000_PWR_MGMT_1;
	mpu_txbuf[1] = 0x03;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	/* Set gyro full range to 2000 dps. We first read in the current register
	 * value so we can change what we need and leave everything else alone. */
	mpu_txbuf[0] = MPU6000_GYRO_CONFIG | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */
	mpu_txbuf[0] = MPU6000_GYRO_CONFIG;
	mpu_txbuf[1] = (mpu_rxbuf[1] & ~0x18) | 0x18;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	/* Set accelerometer full range to 2 g. */
	mpu_txbuf[0] = MPU6000_ACCEL_CONFIG | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */
	mpu_txbuf[0] = MPU6000_ACCEL_CONFIG;
	mpu_txbuf[1] = (mpu_rxbuf[1] & ~0x18) | 0x00;
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 2);   /* Exchange data. */

	read_mpu(dbg_gyr, dbg_acc);   /* Read once to clear out bad data? */
}

void read_mpu(float gyr[3], float acc[3])
{
	static float raw_gyr[3], raw_acc[3];

	/* Read gyroscope. */
	mpu_txbuf[0] = MPU6000_GYRO_XOUT_H | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 7);
	raw_gyr[0] = ((int16_t) ((mpu_rxbuf[1]<<8) | mpu_rxbuf[2])) / 16.384 * M_PI / 180.0 + GYR_X_OFFSET;
	raw_gyr[1] = ((int16_t) ((mpu_rxbuf[3]<<8) | mpu_rxbuf[4])) / 16.384 * M_PI / 180.0 + GYR_Y_OFFSET;
	raw_gyr[2] = ((int16_t) ((mpu_rxbuf[5]<<8) | mpu_rxbuf[6])) / 16.384 * M_PI / 180.0 + GYR_Z_OFFSET;

	/* Read accelerometer. */
	mpu_txbuf[0] = MPU6000_ACCEL_XOUT_H | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 7);
	raw_acc[0] = ((int16_t) ((mpu_rxbuf[1]<<8) | mpu_rxbuf[2])) / 16384.0 + ACC_X_OFFSET;
	raw_acc[1] = ((int16_t) ((mpu_rxbuf[3]<<8) | mpu_rxbuf[4])) / 16384.0 + ACC_Y_OFFSET;
	raw_acc[2] = ((int16_t) ((mpu_rxbuf[5]<<8) | mpu_rxbuf[6])) / 16384.0 + ACC_Z_OFFSET;

	/* Read temperature. */
	mpu_txbuf[0] = MPU6000_TEMP_OUT_H | (1<<7);
	spi_exchange(&SPID1, &mpu_hs_spicfg, mpu_txbuf, mpu_rxbuf, 3);
	mpu_temp = ((int16_t) ((mpu_rxbuf[1]<<8) | mpu_rxbuf[2])) / 340 + 36.53;

	/* Copy sensor reads to debug buffers. */
	uint8_t i;
	for (i=0; i<3; i++) {
		dbg_gyr[i] = raw_gyr[i];
		dbg_acc[i] = raw_acc[i];
	}
	dbg_temp = mpu_temp;

	/* Output */
	gyr[0] = GYR_X;
	gyr[1] = GYR_Y;
	gyr[2] = GYR_Z;
	acc[0] = ACC_X;
	acc[1] = ACC_Y;
	acc[2] = ACC_Z;
}

#define DEBUG_CALIBRATE 0
void debug_mpu(uint8_t *buffer)
{
	static uint16_t count = 1;
	static float g_avg_0 = 0;
	static float g_avg_1 = 0;
	static float g_avg_2 = 0;
	static float a_avg_0 = 0;
	static float a_avg_1 = 0;
	static float a_avg_2 = 0;
	g_avg_0 = (g_avg_0*count + dbg_gyr[0] - GYR_X_OFFSET) / (count+1);
	g_avg_1 = (g_avg_1*count + dbg_gyr[1] - GYR_Y_OFFSET) / (count+1);
	g_avg_2 = (g_avg_2*count + dbg_gyr[2] - GYR_Z_OFFSET) / (count+1);
	a_avg_0 = (a_avg_0*count + dbg_acc[0] - ACC_X_OFFSET) / (count+1);
	a_avg_1 = (a_avg_1*count + dbg_acc[1] - ACC_Y_OFFSET) / (count+1);
	a_avg_2 = (a_avg_2*count + dbg_acc[2] - ACC_Z_OFFSET) / (count+1);

	#if DEBUG_CALIBRATE == 1
	dbg_gyr[0] -= GYR_X_OFFSET;
	dbg_gyr[1] -= GYR_Y_OFFSET;
	dbg_gyr[2] -= GYR_Z_OFFSET;
	dbg_acc[0] -= ACC_X_OFFSET;
	dbg_acc[1] -= ACC_Y_OFFSET;
	dbg_acc[2] -= ACC_Z_OFFSET;
	#endif

	count += 1;

	chsprintf(buffer, "%8u  G: %7d %7d %7d  A: %7d %7d %7d   G_avg: %7d %7d %7d  A_avg: %7d %7d %7d  T: %7d\r\n",
			count,
			(int32_t) (dbg_gyr[0]*1000000),
			(int32_t) (dbg_gyr[1]*1000000),
			(int32_t) (dbg_gyr[2]*1000000),
			(int32_t) (dbg_acc[0]*1000000),
			(int32_t) (dbg_acc[1]*1000000),
			(int32_t) (dbg_acc[2]*1000000),
			(int32_t) (g_avg_0*1000000),
			(int32_t) (g_avg_1*1000000),
			(int32_t) (g_avg_2*1000000),
			(int32_t) (a_avg_0*1000000),
			(int32_t) (a_avg_1*1000000),
			(int32_t) (a_avg_2*1000000),
			(int32_t) (dbg_temp*1000));
}

