#ifndef MPU6000_H
#define MPU6000_H

#include <osuar_spi.h>

#define MPU_SPI_BUFSIZE 512

#define GYR_LSB 1
#define ACC_LSB 1

// TODO: Macros for registers

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

/**
 * @brief Get gyro and accelerometer readings from IMU.
 *
 * @output gyr Rotational rate per axis in rad/s.
 * @output acc Acceleration per axis in g's.
 */
void mpu_read(float gyr[3], float acc[3]);

#endif // MPU6000_H

