#ifndef MOTOR_H
#define MOTOR_H

#include "ch.h"
#include "hal.h"
#include "config.h"

void setup_motors(void);
void update_motors(float, float, float, float);

#if (ESC_COMM == PWM)

/*
 * PWM configuration structure for ESCs using channels 1 through 4 (PD12, PD13,
 * PD14, PD15) of TIM4.
 *
 * See datasheet page 29 for available timers and their capabilities.
 * See datasheet page 45 for pinouts.
 */
static PWMConfig esc_pwm_cfg = {
	1000000,   // 1 MHz PWM clock frequency.
	1000,      // PWM period 1 ms.
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
	},

	0   // HW dependent
};

/*
 * PWM configuration structure for servos using channels 1 through 4 (PB4, PB5,
 * PB0, PB1) of TIM3.
 */
static PWMConfig servo_pwm_cfg = {
	50000,   // 50 kHz PWM clock frequency.
	1000,    // PWM period 20 ms.
	NULL,    // No callback.
	{
#if (NUM_ROTORS == 2)
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
#endif // NUM_ROTORS == 2

#if (NUM_ROTORS == 3)
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
#endif // NUM_ROTORS == 3

#if (NUM_ROTORS == 4)
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
#endif // NUM_ROTORS == 4
	},

	0   // HW dependent
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
	NULL,   // No callback.
	GPIOB,
	0,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPB1 = {
	NULL,   // No callback.
	GPIOB,
	1,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPC4 = {
	NULL,   // No callback.
	GPIOC,
	4,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPC5 = {
	NULL,   // No callback.
	GPIOC,
	5,
	SPI_CR1_DFF
};

#endif // ESC_COMM == SPI

#endif // MOTOR_H

