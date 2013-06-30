// ChibiOS
#include <ch.h>
#include <hal.h>

#include <stdio.h>

// Drivers
#include <osuar_adc.h>   // ADC code
#include <osuar_i2c.h>
#include <osuar_pid.h>   // PID function definition
#include <osuar_mpu6000.h>   // MPU-6000

// Flight controller
#include <osuar_ahrs.h>   // Attitude-Heading Reference System
#include <osuar_uart.h>   // Communications code (wired and wireless)
#include <osuar_motor.h>   // Motor control
#include <osuar_config.h>   // General configuration


/*
 * Communications loop
 */
static WORKING_AREA(wa_comm_thread, 1280);
static msg_t comm_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("communications");
	systime_t time = chTimeNow();
	int counter = 0;

	char txbuf[20];

	float mag[3];

	while (TRUE) {
		time += MS2ST(1000);
		counter++;

		clear_buffer(txbuf);
		sprintf(txbuf, "X: %d  Y: %d  Z: %d\r\n", (int) mag[0], (int) mag[1], (int) mag[2]);
		uartStartSend(&UARTD1, sizeof(txbuf), txbuf);

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * Second communications loop
 */
static WORKING_AREA(wa_comm_thread_2, 512);
static msg_t comm_thread_2(void *arg)
{
	(void) arg;
	chRegSetThreadName("communications 2");
	systime_t time = chTimeNow();
	int counter = 0;

	char txbuf[200];

	while (TRUE) {
		time += MS2ST(100);
		counter++;

		clear_buffer(txbuf);
		debug_ahrs(txbuf);
		uartStartSend(&UARTD3, sizeof(txbuf), txbuf);

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * ADC loop
 */
static WORKING_AREA(wa_adc_thread, 128);
static msg_t adc_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("adc");
	systime_t time = chTimeNow();

	while (TRUE) {
		time += MS2ST(500);

		update_adc();

		uint16_t dutyCycle = avg_ch[3] * 500/4096 + 1;   // TODO: The +1 at the end makes this work. Why?

		palSetPad(GPIOD, 15);
		chThdSleepMilliseconds(dutyCycle);
		palClearPad(GPIOD, 15);

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * Control loop
 */
#define CONTROL_DT 0.005
static WORKING_AREA(wa_control_thread, 128);
static msg_t control_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("control");

	systime_t time = chTimeNow();
	float i[4];
	i[0] = 0;
	i[1] = 1/CONTROL_DT;
	i[2] = 2/CONTROL_DT;
	i[3] = 3/CONTROL_DT;
	uint8_t j = 0;
	float step[4];
	step[0] = CONTROL_DT*1000;
	step[1] = CONTROL_DT*1000;
	step[2] = CONTROL_DT*1000;
	step[3] = CONTROL_DT*1000;

	while (TRUE) {
		time += MS2ST(CONTROL_DT*1000);   // Next deadline in 1 ms.
		for (j=0; j<4; j++) {
			i[j] += step[j];
			if (i[j] > 800.0 || i[j] < 0.0) step[j] = -step[j];
		}

		update_ahrs();

		update_motors(i[0], i[1], i[2], i[3]);

		palTogglePad(GPIOA, 6);

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

	setup_uart();

	setup_adc();

	setup_spi();

	setup_ahrs();

	setup_motors();

	palSetPadMode(GPIOA, 6, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOA, 7, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOA, 6);
	palClearPad(GPIOA, 7);

	/*
	 * Short delay to let the various setup functions finish.
	 */
	chThdSleepMilliseconds(1);

	/*
	 * Create the communications thread.
	 */
	chThdCreateStatic(wa_comm_thread, sizeof(wa_comm_thread), NORMALPRIO, comm_thread, NULL);

	/*
	 * Create the second communications thread.
	 */
	chThdCreateStatic(wa_comm_thread_2, sizeof(wa_comm_thread_2), NORMALPRIO, comm_thread_2, NULL);

	/*
	 * Create the ADC thread.
	 */
	chThdCreateStatic(wa_adc_thread, sizeof(wa_adc_thread), NORMALPRIO, adc_thread, NULL);

	/*
	 * Create control thread.
	 */
	chThdCreateStatic(wa_control_thread, sizeof(wa_control_thread), HIGHPRIO, control_thread, NULL);

	while (TRUE) {
	}

	return 0;
}

