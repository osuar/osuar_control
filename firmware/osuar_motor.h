#ifndef MOTOR_H
#define MOTOR_H

#include <ch.h>
#include <hal.h>
#include <osuar_config.h>

void setup_motors(void);
void update_motors(float, float, float, float);

/*
 * SPI end transfer callback.
 */
static void spicb(SPIDriver*);


#if (ESC_COMM == PWM)

/*
 * PWM configuration structure for ESCs using channels 1 through 4 (PC6, PC7,
 * PC8, PC9) of TIM8.
 *
 * See datasheet page 29 for available timers and their capabilities.
 * See datasheet page 45 for pinouts.
 */
static PWMConfig pwm8cfg = {
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
 * PWM configuration structures for servos using one or both of:
 * PA8 - TIM1 channel 1
 * PB5 - TIM3 channel 2
 */
#if (NUM_ROTORS < 4)
static PWMConfig pwm1cfg = {
	50000,   // 50 kHz PWM clock frequency.
	1000,    // PWM period 20 ms.
	NULL,    // No callback.
	{
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
	},

	0   // HW dependent
};
#endif // NUM_ROTORS < 4

#if (NUM_ROTORS < 3)
static PWMConfig pwm3cfg = {
	50000,   // 50 kHz PWM clock frequency.
	1000,    // PWM period 20 ms.
	NULL,    // No callback.
	{
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
	},

	0   // HW dependent
};
#endif // NUM_ROTORS < 3

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
	spicb,
	GPIOB,
	0,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPB1 = {
	spicb,
	GPIOB,
	1,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPC4 = {
	spicb,
	GPIOC,
	4,
	SPI_CR1_DFF
};

static const SPIConfig spi3cfgPC5 = {
	spicb,
	GPIOC,
	5,
	SPI_CR1_DFF
};

#endif // ESC_COMM == SPI

#endif // MOTOR_H

