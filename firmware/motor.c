#include <motor.h>

/**
 * Set up pins, etc.
 */
void setup_motors()
{
	/*
	 * Configure servo timer.
	 */
	pwmStart(&PWMD3, &servo_pwm_cfg);

	/*
	 * Set up servo pins.
	 */
#if (NUM_ROTORS < 4)
	palSetPadMode(GPIOB, GPIOB_PIN0, PAL_MODE_ALTERNATE(2));
#endif // NUM_ROTORS < 4

#if (NUM_ROTORS < 3)
	palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_ALTERNATE(2));
#endif // NUM_ROTORS < 3

	//palSetPadMode(GPIOB, GPIOB_PIN4, PAL_MODE_ALTERNATE(2));
	//palSetPadMode(GPIOB, GPIOB_PIN5, PAL_MODE_ALTERNATE(2));


#if (ESC_COMM == PWM)
	/*
	 * Configure ESC timer.
	 */
	pwmStart(&PWMD4, &esc_pwm_cfg);

	/*
	 * Set up ESC pins.
	 */
	palSetPadMode(GPIOD, GPIOD_LED3, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_ALTERNATE(2));

#if (NUM_ROTORS > 2)
	palSetPadMode(GPIOD, GPIOD_LED5, PAL_MODE_ALTERNATE(2));
#endif // NUM_ROTORS > 2

#if (NUM_ROTORS > 3)
	palSetPadMode(GPIOD, GPIOD_LED6, PAL_MODE_ALTERNATE(2));
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
#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, a));   // Motor 1
	pwmEnableChannel(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, b));   // Motor 2
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
#endif // ESC_COMM == SPI

	pwmEnableChannel(&PWMD3, 0, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, c));   // Servo 1
	pwmEnableChannel(&PWMD3, 1, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, d));   // Servo 2
#endif // NUM_ROTORS == 2

	/*
	 * Commands for three-rotor system.
	 */
#if (NUM_ROTORS == 3)
#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, a));   // Motor 1
	pwmEnableChannel(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, b));   // Motor 2
	pwmEnableChannel(&PWMD4, 2, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, c));   // Motor 3
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
#endif // ESC_COMM == SPI

	pwmEnableChannel(&PWMD3, 0, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, c));   // Servo 1
#endif // NUM_ROTORS == 3

	/*
	 * Commands for four-rotor system.
	 */
#if (NUM_ROTORS == 4)
	uint16_t mot1, mot2, mot3, mot4;
	mot1 = (uint16_t) a;
	mot2 = (uint16_t) b;
	mot3 = (uint16_t) c;
	mot4 = (uint16_t) d;

#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, mot1));   // Motor 1
	pwmEnableChannel(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, mot2));   // Motor 2
	pwmEnableChannel(&PWMD4, 2, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, mot3));   // Motor 3
	pwmEnableChannel(&PWMD4, 3, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, mot4));   // Motor 4
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
#endif // ESC_COMM == SPI
#endif // NUM_ROTORS == 4

	//pwmEnableChannel(&PWMD3, 2, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, i));
	//pwmEnableChannel(&PWMD3, 3, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, i));
}


