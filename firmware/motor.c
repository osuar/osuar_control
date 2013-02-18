#include <motor.h>

/**
 * Set up pins, etc.
 */
void setup_motors()
{
#if (ESC_COMM == PWM)
	/*
	 * Enable timers.
	 */
	pwmStart(&PWMD4, &esc_pwm_cfg);
	pwmStart(&PWMD3, &servo_pwm_cfg);

	/*
	 * Set up pins.
	 */
	palSetPadMode(GPIOD, GPIOD_LED3, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, GPIOD_LED5, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, GPIOD_LED6, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOB, GPIOB_PIN0, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOB, GPIOB_PIN4, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOB, GPIOB_PIN5, PAL_MODE_ALTERNATE(2));
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)

#endif // ESC_COMM == SPI
}

/**
 * Send new desired motor speeds to ESCs.
 */
void update_motors()
{
#if defined(ESC_PWM)
	pwmEnableChannel(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, i));
	pwmEnableChannel(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, i));
	pwmEnableChannel(&PWMD4, 2, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, i));
	pwmEnableChannel(&PWMD4, 3, PWM_FRACTION_TO_WIDTH(&PWMD4, 1000, i));
	pwmEnableChannel(&PWMD3, 0, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, i));
	pwmEnableChannel(&PWMD3, 1, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, i));
	pwmEnableChannel(&PWMD3, 2, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, i));
	pwmEnableChannel(&PWMD3, 3, PWM_FRACTION_TO_WIDTH(&PWMD3, 1000, i));
#endif // ESC_PWM

#if defined(ESC_SPI)

#endif // ESC_SPI
}


