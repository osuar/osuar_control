// ChibiOS
#include <ch.h>
#include <hal.h>

#include <stdio.h>

// Drivers
#include <i2c.h>
#include <pid.h>   // PID function definition
#include <mpu6050.h>   // MPU-6050

// Flight controller
#include <ahrs.h>   // Attitude-Heading Reference System
#include <comm.h>   // Communications code (wired and wireless)
#include <motor.h>   // Motor control
#include <config.h>   // General configuration


/*
 * Blink orange LED.
 */
static WORKING_AREA(wa_led_thread, 128);
static msg_t led_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("blinker");
	systime_t time = chTimeNow();

	while (TRUE) {
		time += MS2ST(1000);   // Next deadline in 1 second.

		palSetPad(GPIOD, GPIOD_LED3);
		chThdSleepMilliseconds(50);
		palClearPad(GPIOD, GPIOD_LED3);
		chThdSleepMilliseconds(100);
		palSetPad(GPIOD, GPIOD_LED3);
		chThdSleepMilliseconds(50);
		palClearPad(GPIOD, GPIOD_LED3);

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * Communications loop
 */
static WORKING_AREA(wa_comm_thread, 128);
static msg_t comm_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("communications");
	systime_t time = chTimeNow();
	int counter = 0;

	char txbuf[20];

	while (TRUE) {
		time += MS2ST(100);   // Next deadline in 1 second.
		counter++;

		sprintf(txbuf, "Je vis!\r\n");
		uartStartSend(&UARTD2, sizeof(txbuf), txbuf);

		palSetPad(GPIOD, GPIOD_LED4);
		chThdSleepMilliseconds(50);
		palClearPad(GPIOD, GPIOD_LED4);

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * Second communications loop
 */
static WORKING_AREA(wa_comm_thread_2, 128);
static msg_t comm_thread_2(void *arg)
{
	(void) arg;
	chRegSetThreadName("communications 2");
	systime_t time = chTimeNow();
	int counter = 0;

	char txbuf[20];

	while (TRUE) {
		time += MS2ST(234);   // Next deadline in 1 second.
		counter++;

		sprintf(txbuf, "Je vis aussi!\r\n");
		uartStartSend(&UARTD3, sizeof(txbuf), txbuf);

		palSetPad(GPIOD, GPIOD_LED5);
		chThdSleepMilliseconds(50);
		palClearPad(GPIOD, GPIOD_LED5);

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * Control loop
 */
static WORKING_AREA(wa_control_thread, 128);
static msg_t control_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("control");

	setup_motors();

	systime_t time = chTimeNow();
	float i = 0;
	float dir = 0.2;

	while (TRUE) {
		time += MS2ST(1);   // Next deadline in 1 ms.
		i += dir;
		if (i > 1000.0) dir = -0.2;
		if (i < 0.0) dir = 0.2;

		update_ahrs();

		update_motors(i, i, i, i);

		chThdSleepUntil(time);
	}

	return 0;
}


/*
 * Application entry point.
 */
int main(void)
{
	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	setup_comm();   // TODO: This hangs thread, I think.

	/*
	 * Initialize ADC driver 1 and set the following as inputs:
	 * PA4, PA5, PC0, PC1, PC2, PC3
	 */
	adcStart(&ADCD1, NULL);
	palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(GPIOA, 5, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(GPIOC, 1, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(GPIOC, 2, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(GPIOC, 3, PAL_MODE_INPUT_ANALOG);

	/*
	 * Short delay to let the various setup functions finish.
	 */
	chThdSleepMilliseconds(1);

	/*
	 * Create the LED thread.
	 */
	chThdCreateStatic(wa_led_thread, sizeof(wa_led_thread), NORMALPRIO, led_thread, NULL);

	/*
	 * Create the communications thread.
	 */
	chThdCreateStatic(wa_comm_thread, sizeof(wa_comm_thread), NORMALPRIO, comm_thread, NULL);

	/*
	 * Create the second communications thread.
	 */
	chThdCreateStatic(wa_comm_thread_2, sizeof(wa_comm_thread_2), NORMALPRIO, comm_thread_2, NULL);

	/*
	 * Create control thread.
	 */
	chThdCreateStatic(wa_control_thread, sizeof(wa_control_thread), HIGHPRIO, control_thread, NULL);

	while (TRUE) {
	}

	return 0;
}

