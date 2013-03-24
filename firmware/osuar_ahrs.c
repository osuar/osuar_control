#include <osuar_ahrs.h>

void setup_ahrs(void)
{
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

