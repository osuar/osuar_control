#include <osuar_ahrs.h>

void setup_ahrs(void)
{
	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4));   // I2C1_SCL
	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4));   // I2C1_SDA
	i2cStart(&I2CD1, &i2c1cfg);

	//init_accel();   // TODO
	//init_gyro();   // TODO
	init_mag();
}

void update_ahrs(void)
{
	//poll_accel();   // TODO
	//poll_gyro();   // TODO
	poll_mag();

}

