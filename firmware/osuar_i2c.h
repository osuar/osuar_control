#ifndef I2C_H
#define I2C_H

#define OSUAR_I2C_TIMEOUT 5   // CH_FREQUENCY == 10000, so this equals 50 us.

/*
 * I2C configuration structure.
 *
 * See $CHIBIOS/os/hal/platforms/STM32/I2Cv1
 */
static const I2CConfig i2c1cfg = {
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2
};

#endif // I2C_H

