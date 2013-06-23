#include <osuar_i2c.h>

void setup_i2c(void)
{
	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4));   // I2C1_SCL
	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4));   // I2C1_SDA

	i2cStart(&I2CD1, &i2c1cfg);
}

