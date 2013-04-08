#include <osuar_mag_lsm303.h>

int mag_id;

static uint8_t mag_tx_data[8];
static uint8_t mag_rx_data[8];

static int16_t mRaw[3];   // Raw 16-bit readings of magnetometer.
static float mVec[3];   // Vector to magnetic north.

msg_t mag_transmit(uint8_t *txbuf, size_t txsize, uint8_t *rxbuf, size_t rxsize);
msg_t mag_receive(uint8_t *rxbuf, size_t rxsize);


msg_t mag_transmit(uint8_t* txbuf, size_t txsize, uint8_t* rxbuf, size_t rxsize)
{
	static msg_t status;
	static i2cflags_t errors;

	i2cAcquireBus(&I2CD1);
	status = i2cMasterTransmitTimeout(&I2CD1,
			MAG_LSM303_ADDRESS,
			txbuf,   // TX buffer
			txsize,   // Number of bytes to send
			rxbuf,   // RX buffer
			rxsize,   // Number of bytes to read
			OSUAR_I2C_TIMEOUT);   // Number of ticks before timeout
	i2cReleaseBus(&I2CD1);

	/* Get error flags. Currently, we don't do anything with them. */
	if (status != RDY_OK) {
		errors = i2cGetErrors(&I2CD1);
	}

	return status;
}

msg_t mag_receive(uint8_t* rxbuf, size_t rxsize)
{
	static msg_t status;
	static i2cflags_t errors;

	i2cAcquireBus(&I2CD1);
	status = i2cMasterReceiveTimeout(&I2CD1,
			MAG_LSM303_ADDRESS,
			rxbuf,   // RX buffer
			rxsize,   // Number of bytes to read
			OSUAR_I2C_TIMEOUT);   // Number of ticks before timeout
	i2cReleaseBus(&I2CD1);

	/* Get error flags. Currently, we don't do anything with them. */
	if (status != RDY_OK) {
		errors = i2cGetErrors(&I2CD1);
	}

	return status;
}

void init_mag(void)
{
	mag_tx_data[0] = 0x0f;
	mag_transmit(mag_tx_data, 1, mag_rx_data, 1);
	mag_id = mag_rx_data[0];

	// Grab default values from magnetometer.
	mag_tx_data[0] = 0x00;
	mag_transmit(mag_tx_data, 1, mag_rx_data, 3);

	// TODO: error check!
	// Data rate 75 Hz (DS p. 33).
	mag_tx_data[1] = (mag_rx_data[0] & ~(7<<2)) | (6<<2);

	// Gain 670 LSB/gauss (DS p. 34).
	mag_tx_data[2] = (mag_rx_data[1] & ~(7<<5)) | (3<<5);

	// Continuous-conversion mode (DS p. 34).
	mag_tx_data[3] = (mag_rx_data[2] & ~(3)) | 0;

	// Write to registers.
	mag_transmit(mag_tx_data, 4, mag_rx_data, 0);
}

void poll_mag(void)
{
	// Read magnetometer values.
	mag_tx_data[0] = 0x03;
	mag_transmit(mag_tx_data, 1, mag_rx_data, 6);

	// Convert to 2's complement.
	mRaw[0] = ((mag_rx_data[0] << 8) | mag_rx_data[1]);   // X
	mRaw[1] = ((mag_rx_data[4] << 8) | mag_rx_data[5]);   // Y
	mRaw[2] = ((mag_rx_data[2] << 8) | mag_rx_data[3]);   // Z

	// Convert to float and account for offset.
	mVec[0] = ((float) (mRaw[0] - MAG_X_MIN)) / (MAG_X_MAX - MAG_X_MIN) * 2 - 1;
	mVec[1] = ((float) (mRaw[1] - MAG_Y_MIN)) / (MAG_Y_MAX - MAG_Y_MIN) * 2 - 1;
	mVec[2] = ((float) (mRaw[2] - MAG_Z_MIN)) / (MAG_Z_MAX - MAG_Z_MIN) * 2 - 1;
}

void get_mag(float output[3])
{
	int i;
	for (i=0; i<3; i++) {
		//output[i] = mVec[i];
		output[i] = (float) mRaw[i];
	}
}

