#ifndef MAG_LSM303_H
#define MAG_LSM303_H

#define MAG_LSM303_MAG_ADDR 0x1e

// TODO: These are calibration values from old code. Recalibrate.
#define MAG_X_MIN -314
#define MAG_X_MAX 320
#define MAG_Y_MIN -316
#define MAG_Y_MAX 317
#define MAG_Z_MIN -427
#define MAG_Z_MAX 165

static const uint8_t mag_tx_data[8];
static const uint8_t mag_rx_data[8];

static const int16_t mRaw[3];   // Vector to magnetic north.
static const float mVec[3];   // Vector to magnetic north.

msg_t mag_transmit(uint8_t *txbuf, size_t txsize, uint8_t *rxbuf, size_t rxsize);
msg_t mag_receive(uint8_t *rxbuf, size_t rxsize);
void init_mag(void);
void poll_mag(void);
void get_mag(void);

#endif // MAG_LSM303_H

