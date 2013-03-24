#ifndef I2C_H
#define I2C_H

#define OSUAR_I2C_TIMEOUT 16800   // TODO: 16800 ticks.. does this equal a timeout of 100 us?

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

