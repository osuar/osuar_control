#include <osuar_motor.h>

#include <hal.h>

#include <osuar_config.h>

#if (ESC_COMM == PWM)

/*
 * PWM configuration structure for ESCs using channels 1 through 4 (PC6, PC7,
 * PC8, PC9) of TIM8.
 *
 * See datasheet page 29 for available timers and their capabilities.
 * See datasheet page 45 for pinouts.
 */
static PWMConfig pwm8cfg = {
	500000,    // 500 kHz PWM clock frequency.
	1000,      // PWM period 2.0 ms.
	NULL,      // No callback.
	{
#if (NUM_ROTORS == 2)
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
#endif // NUM_ROTORS == 2

#if (NUM_ROTORS == 3)
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
#endif // NUM_ROTORS == 3

#if (NUM_ROTORS == 4)
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}
#endif // NUM_ROTORS == 4
	},   // Channel configurations

	0,0   // HW dependent
};

/*
 * PWM configuration structures for servos using:
 * PA8 - TIM1 channel 1 (enabled always)
 * PB0 - TIM3 channel 3
 * PB1 - TIM3 channel 4
 *
 * Channels 3 and 4 would normally be used for SPI, but currently on the
 * dualrotor, this is not the case.
 */
static PWMConfig pwm1cfg = {
	50000,   // 50 kHz PWM clock frequency.
	1000,    // PWM period 20 ms.
	NULL,    // No callback.
	{
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
	},   // Channel configurations

	0,0   // HW dependent
};

static PWMConfig pwm3cfg = {
	50000,   // 50 kHz PWM clock frequency.
	1000,    // PWM period 20 ms.
	NULL,    // No callback.
	{
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},

#if (NUM_ROTORS == 2)
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}
#endif // NUM_ROTORS == 2

#if (NUM_ROTORS == 3)
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
#endif // NUM_ROTORS == 3
	},   // Channel configurations

	0,0   // HW dependent
};

#endif // ESC_COMM == PWM


#if (ESC_COMM == SPI)

/*
 * SPI3 configuration structures.
 *
 * These separate configurations are used in calling spiStart() repeatedly in
 * order to reprogram the SPI on the fly to address multiple slaves. This is
 * not a workaround, according to this:
 * http://forum.chibios.org/phpbb/viewtopic.php?f=2&t=950#p9066
 *
 * Speed 21 MHz, CPHA = 0, CPOL = 0, 16 bits frames, MSb transmitted first.
 * The slave select lines are PB0, PB1, PC4, PC5.
 */
static const SPIConfig spi3cfgPB0 = {
	NULL,
	GPIOB,
	0,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPB1 = {
	NULL,
	GPIOB,
	1,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPC4 = {
	NULL,
	GPIOC,
	4,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPC5 = {
	NULL,
	GPIOC,
	5,
	SPI_CR1_DFF
};

#endif // ESC_COMM == SPI


void setup_motors()
{
	/*
	 * Set up servo pins.
	 */
	pwmStart(&PWMD1, &pwm1cfg);
	palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(1));

#if (NUM_ROTORS < 4)
	pwmStart(&PWMD3, &pwm3cfg);
	palSetPadMode(GPIOB, 0, PAL_MODE_ALTERNATE(2));
#endif // NUM_ROTORS < 4

#if (NUM_ROTORS < 3)
	palSetPadMode(GPIOB, 1, PAL_MODE_ALTERNATE(2));
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
	 * Initialize SPI CS lines.
	 * PC4, PC5, PB0, PB1 - slave select lines
	 */

	palSetPadMode(GPIOC, 4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(GPIOC, 4);
	palSetPadMode(GPIOC, 5, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(GPIOC, 5);

#if (NUM_ROTORS > 2)
	palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(GPIOB, 0);
#endif // NUM_ROTORS > 2

#if (NUM_ROTORS > 3)
	palSetPadMode(GPIOB, 1, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(GPIOB, 1);
#endif // NUM_ROTORS > 3

#endif // ESC_COMM == SPI
}

void update_motors(float dc[4])
{
	(void) dc;
	/*
	 * Commands for two-rotor system.
	 */
#if (NUM_ROTORS == 2)
#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD8, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[0]) * 10000));
	pwmEnableChannel(&PWMD8, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[1]) * 10000));
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
	// TODO: send data to motor0.
	// TODO: send data to motor1.
#endif // ESC_COMM == SPI

	pwmEnableChannel(&PWMD3, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, (SERVO_PWM_MIN + (SERVO_PWM_MAX-SERVO_PWM_MIN) * dc[2]) * 10000));
	pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, (SERVO_PWM_MIN + (SERVO_PWM_MAX-SERVO_PWM_MIN) * dc[3]) * 10000));
#endif // NUM_ROTORS == 2

	/*
	 * Commands for three-rotor system.
	 */
#if (NUM_ROTORS == 3)
#if (ESC_COMM == PWM)
	pwmEnableChannel(&PWMD8, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[0]) * 10000));
	pwmEnableChannel(&PWMD8, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[1]) * 10000));
	pwmEnableChannel(&PWMD8, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[2]) * 10000));
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
	// TODO: send data to motor0.
	// TODO: send data to motor1.
	// TODO: send data to motor2.
#endif // ESC_COMM == SPI

	pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, (SERVO_PWM_MIN + (SERVO_PWM_MAX-SERVO_PWM_MIN) * dc[3]) * 10000));
#endif // NUM_ROTORS == 3

	/*
	 * Commands for four-rotor system.
	 */
#if (NUM_ROTORS == 4)
	/*
	 * Calculate motor values.
	 */
#if (ESC_COMM == PWM)
	// Duty cycle from 0 to 10000.
	pwmEnableChannel(&PWMD8, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[0])*10000));
	pwmEnableChannel(&PWMD8, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[1])*10000));
	pwmEnableChannel(&PWMD8, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[2])*10000));
	pwmEnableChannel(&PWMD8, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD8, (MOTOR_PWM_MIN + (MOTOR_PWM_MAX-MOTOR_PWM_MIN) * dc[3])*10000));
#endif // ESC_COMM == PWM

#if (ESC_COMM == SPI)
	// TODO: send data to motor0.
	// TODO: send data to motor1.
	// TODO: send data to motor2.
	// TODO: send data to motor3.
#endif // ESC_COMM == SPI

#endif // NUM_ROTORS == 4
}

