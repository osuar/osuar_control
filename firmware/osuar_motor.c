#include <osuar_motor.h>

/**
 * Set up pins, etc.
 */
void setup_motors()
{
	/*
	 * Set up servo pins.
	 */
#if (NUM_ROTORS < 4)
	pwmStart(&PWMD1, &pwm1cfg);
	palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(2));
#endif // NUM_ROTORS < 4

#if (NUM_ROTORS < 3)
	pwmStart(&PWMD3, &pwm3cfg);
	palSetPadMode(GPIOB, 5, PAL_MODE_ALTERNATE(2));
#endif // NUM_ROTORS < 3

	/*
	 * Set up ESC pins.
	 */
#if (ESC_COMM == PWM)
	pwmStart(&PWMD8, &pwm8cfg);
	palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(3));
	palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(3));

#if (NUM_ROTORS > 2)
	palSetPadMode(GPIOC, 8, PAL_MODE_ALTERNATE(3));
#endif // NUM_ROTORS > 2

#if (NUM_ROTORS > 3)
	palSetPadMode(GPIOC, 9, PAL_MODE_ALTERNATE(3));
#endif // NUM_ROTORS > 3

#endif // ESC_COMM == PWM


#if (ESC_COMM == SPI)

	/*
	 * Initialize SPI driver 3.
	 * PB0, PB1, PC4, PC5 - slave select lines
	 * PC10 - SCK
	 * PC11 - MISO
	 * PC12 - MOSI
	 */
	//spiStart(&SPID3, &spi3cfg);   // TODO: Do I need to call this before palSetPad() and such?
	palSetPadMode(GPIOC, 10, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(GPIOC, 11, PAL_MODE_ALTERNATE(5));
	palSetPadMode(GPIOC, 12, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);

	palSetPad(GPIOB, 0);
	palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(GPIOB, 1);
	palSetPadMode(GPIOB, 1, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

#if (NUM_ROTORS > 2)
	palSetPad(GPIOC, 4);
	palSetPadMode(GPIOC, 4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
#endif // NUM_ROTORS > 2

#if (NUM_ROTORS > 3)
	palSetPad(GPIOC, 5);
	palSetPadMode(GPIOC, 5, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
#endif // NUM_ROTORS > 3

#endif // ESC_COMM == SPI
}

/**
 * Send new desired motor speeds to ESCs.
 */
void update_motors(float a, float b, float c, float d)
{
	/*
	 * Commands for two-rotor system.
	 */
#if (NUM_ROTORS == 2)
	uint16_t motor0, motor1, servo0, servo1;

	/*
	 * Calculate motor and servo values.
	 */
	motor0 = (uint16_t) a;
	motor1 = (uint16_t) b;
	servo0 = (uint16_t) c;
	servo1 = (uint16_t) d;

#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD8, 0, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor0));
	pwmEnableChannel(&PWMD8, 1, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor1));
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
	spiStart(&SPID3, &spi3cfgPB0);
	// TODO: send data to motor0.
	spiStart(&SPID3, &spi3cfgPB1);
	// TODO: send data to motor1.
#endif // ESC_COMM == SPI

	pwmEnableChannel(&PWMD1, 0, PWM_FRACTION_TO_WIDTH(&PWMD1, 1000, servo0));
	pwmEnableChannel(&PWMD3, 1, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, servo1));
#endif // NUM_ROTORS == 2

	/*
	 * Commands for three-rotor system.
	 */
#if (NUM_ROTORS == 3)
	uint16_t motor0, motor1, motor2, servo0;

	/*
	 * Calculate motor and servo values.
	 */
	motor0 = (uint16_t) a;
	motor1 = (uint16_t) b;
	motor2 = (uint16_t) c;
	servo0 = (uint16_t) d;

#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD8, 0, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor0));
	pwmEnableChannel(&PWMD8, 1, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor1));
	pwmEnableChannel(&PWMD8, 2, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor2));
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
	spiStart(&SPID3, &spi3cfgPB0);
	// TODO: send data to motor0.
	spiStart(&SPID3, &spi3cfgPB1);
	// TODO: send data to motor1.
	spiStart(&SPID3, &spi3cfgPC4);
	// TODO: send data to motor2.
#endif // ESC_COMM == SPI

	pwmEnableChannel(&PWMD1, 0, PWM_FRACTION_TO_WIDTH(&PWMD1, 1000, servo0));
#endif // NUM_ROTORS == 3

	/*
	 * Commands for four-rotor system.
	 */
#if (NUM_ROTORS == 4)
	uint16_t motor0, motor1, motor2, motor3;

	/*
	 * Calculate motor values.
	 */
	motor0 = (uint16_t) a;
	motor1 = (uint16_t) b;
	motor2 = (uint16_t) c;
	motor3 = (uint16_t) d;

#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD8, 0, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor0));
	pwmEnableChannel(&PWMD8, 1, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor1));
	pwmEnableChannel(&PWMD8, 2, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor2));
	pwmEnableChannel(&PWMD8, 3, PWM_FRACTION_TO_WIDTH(&PWMD8, 1000, motor3));
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
	spiStart(&SPID3, &spi3cfgPB0);
	// TODO: send data to motor0.
	spiStart(&SPID3, &spi3cfgPB1);
	// TODO: send data to motor1.
	spiStart(&SPID3, &spi3cfgPC4);
	// TODO: send data to motor2.
	spiStart(&SPID3, &spi3cfgPC5);
	// TODO: send data to motor3.
#endif // ESC_COMM == SPI

#endif // NUM_ROTORS == 4
}

/*
 * SPI end transfer callback.
 */
static void spicb(SPIDriver *spip)
{
	chSysLockFromIsr();
	spiUnselectI(spip);   // Release slave select line.
	chSysUnlockFromIsr();
}

