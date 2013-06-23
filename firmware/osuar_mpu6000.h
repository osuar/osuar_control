#ifndef MPU6000_H
#define MPU6000_H

#include <osuar_spi.h>
#include <osuar_uart.h>

#define MPU_SPI_BUFSIZE 14

#define GYR_LSB 1
#define ACC_LSB 1

// TODO: Macros for registers

/**
 * @brief Set up MPU-6000 as SPI slave.
 */
void setup_mpu(void);

/**
 * @brief Get gyro and accelerometer readings from IMU.
 *
 * @output gyr Rotational rate per axis in rad/s.
 * @output acc Acceleration per axis in g's.
 */
void read_mpu(float gyr[3], float acc[3]);

#endif // MPU6000_H

