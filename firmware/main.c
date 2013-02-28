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
	float i = 0;
	float dir = 0.2;

	while (TRUE) {
		time += MS2ST(1);   // Next deadline in 1 ms.
		i += dir;
		if (i > 1000.0) dir = -0.2;
		if (i < 0.0) dir = 0.2;

		// TODO: Update IMU.

		update_motors(i, i, i, i);

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
	 * Activate USART2 and USART4
	 */
	sdStart(&SD2, NULL);
	sdStart(&SD4, NULL);
	palSetPadMode(GPIOA, 0, PAL_MODE_ALTERNATE(7));   // USART4 TX
	palSetPadMode(GPIOA, 1, PAL_MODE_ALTERNATE(7));   // USART4 RX
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));   // USART2 TX
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));   // USART2 RX

	/*
	 * Initialize SPI driver 3.
	 * PA15 - NSS
	 * PC10 - SCK
	 * PC11 - MISO
	 * PC12 - MOSI
	 */
	spiStart(&SPID3, &spi3cfg);
	palSetPad(GPIOA, 15);
	palsetPadMode(GPIOA, 15, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(GPIOC, 10, PAL_MODE_ALTERNATE(5)    | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(GPIOC, 11, PAL_MODE_ALTERNATE(5));
	palSetPadMode(GPIOC, 12, PAL_MODE_ALTERNATE(5)    | PAL_STM32_OSPEED_HIGHEST);

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

