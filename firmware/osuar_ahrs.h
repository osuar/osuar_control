#ifndef AHRS_H
#define AHRS_H

#include <ch.h>
#include <hal.h>
#include <osuar_config.h>

void setup_ahrs(void);
void update_ahrs(void);

/*
 * I2C configuration structure.
 */
static const I2CConfig i2c1cfg = {
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2,
};

#endif // AHRS_H

