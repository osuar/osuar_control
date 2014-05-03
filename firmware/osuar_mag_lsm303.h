#ifndef MAG_LSM303_H
#define MAG_LSM303_H

#define MAG_LSM303_ADDRESS 0x1e

// TODO: These are calibration values from old code. Recalibrate.
#define MAG_X_MIN -314
#define MAG_X_MAX 320
#define MAG_Y_MIN -316
#define MAG_Y_MAX 317
#define MAG_Z_MIN -427
#define MAG_Z_MAX 165

void init_mag(void);
void poll_mag(void);
void get_mag(float output[3]);

#endif // MAG_LSM303_H

