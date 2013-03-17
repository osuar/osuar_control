#ifndef MPU6050_H
#define MPU6050_H

#define IMU_RX_DEPTH 8
#define IMU_TX_DEPTH 8

// TODO: Macros for registers

uint16_t imu_rx_data[IMU_RX_DEPTH];
uint16_t imu_tx_data[IMU_TX_DEPTH];

/*
 * Configuration struct for MPU6050.
 */
static const I2CSlaveConfig mpucfg = {
	NULL,
	NULL,
	&imu_rx_data[0],
	IMU_RX_DEPTH,
	0,
	0,
	&imu_tx_data[0],
	IMU_TX_DEPTH,
	0,
	0,
	0b0011101,   // TODO: I don't know what this does
	I2C_WRITE,
	FALSE
};

#endif // MPU6050_H

