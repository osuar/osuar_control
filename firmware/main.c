// ChibiOS
#include "ch.h"
#include "hal.h"

// Drivers
#include "i2c.h"
#include "pid.h"   // PID function definition
#include "mpu6050.h"   // MPU-6050

// Flight controller
#include "ahrs.h"   // Attitude-Heading Reference System
#include "comm.h"   // Communications code (wired and wireless)
#include "motor.h"   // Motor control
#include "config.h"   // General configuration


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
	uint32_t i = 0;
	int dir = 1;

	while (TRUE) {
		time += MS2ST(1);   // Next deadline in 1 ms.
		i += dir;
		if (i == 1000) dir = -1;
		if (i == 0) dir = 1;

		// TODO: Update IMU.

		update_motors();

		chThdSleepUntil(time);
	}
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

	/*
	 * Create the LED thread.
	 */
	chThdCreateStatic(wa_led_thread, sizeof(wa_led_thread), NORMALPRIO, led_thread, NULL);

	/*
	 * Create control thread.
	 */
	chThdCreateStatic(wa_control_thread, sizeof(wa_control_thread), HIGHPRIO, control_thread, NULL);

	while (TRUE) {
	}

	return 0;
}

