#ifndef I2C_H
#define I2C_H

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

