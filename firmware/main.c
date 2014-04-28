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
#include <osuar_comm.h>   // Communication
#include <osuar_uart.h>   // Communications code (wired and wireless)
#include <osuar_motor.h>   // Motor control
#include <osuar_config.h>   // General configuration


static float dbg_dcm[3][3];
static float throttle = 0.10;
static float new_des_ang_pos[3];
static uint32_t counter = 0;

/*
 * Communications loop
 */
static WORKING_AREA(wa_comm_thread, 1280);
static msg_t comm_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("communications");
	systime_t time = chTimeNow();

	uint8_t txbuf[200];

	while (TRUE) {
		time += MS2ST(11)-1;

		// clear_buffer(txbuf);
		// chsprintf(txbuf, "%9d   %5d %5d %5d   %5d %5d %5d   %5d %5d %5d\r\n",
		// 		counter,
		// 		(int16_t) (dbg_dcm[0][0]*1000),
		// 		(int16_t) (dbg_dcm[0][1]*1000),
		// 		(int16_t) (dbg_dcm[0][2]*1000),
		// 		(int16_t) (dbg_dcm[1][0]*1000),
		// 		(int16_t) (dbg_dcm[1][1]*1000),
		// 		(int16_t) (dbg_dcm[1][2]*1000),
		// 		(int16_t) (dbg_dcm[2][0]*1000),
		// 		(int16_t) (dbg_dcm[2][1]*1000),
		// 		(int16_t) (dbg_dcm[2][2]*1000));
		// uartStartSend(&UARTD1, sizeof(txbuf), txbuf);

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * Remote control loop
 */
static int ticks_since_last_comm = 0;
static WORKING_AREA(wa_comm_thread_2, 512);
static msg_t comm_thread_2(void *arg)
{
	(void) arg;
	chRegSetThreadName("remote comm");
	systime_t time = chTimeNow();

	uartStartReceive(&UARTD3, sizeof(remote_comm_rxbuf), remote_comm_rxbuf);

	while (TRUE) {
		time += MS2ST(11)-1;

		/* Receive */
		if(osuar_comm_parse_input(&throttle, new_des_ang_pos)) {
                  ticks_since_last_comm = 0;
                } else {
                  ticks_since_last_comm++;

                  if(ticks_since_last_comm > 100) {
                    throttle = 0;
                  }
                }

		/* Transmit */
		clear_buffer(remote_comm_txbuf);   // TODO(yoos): maybe check whether or not we've finished transmitting before clearing buffer

		chsprintf(remote_comm_txbuf, "%d\r\n", ticks_since_last_comm);
		uartStartSend(&UARTD3, sizeof(remote_comm_txbuf), remote_comm_txbuf);

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
		time += MS2ST(50);

		update_adc();

		uint16_t dutyCycle = avg_ch[3] * 500/4096 + 1;   // TODO: The +1 at the end makes this work. Why?

		chThdSleepUntil(time);
	}

	return 0;
}

/*
 * Control loop
 */
static WORKING_AREA(wa_control_thread, 1280);
static msg_t control_thread(void *arg)
{
	(void) arg;
	chRegSetThreadName("control");
	systime_t time = chTimeNow();

	/* DCM of body in global frame. */
	float dcm_bg[3][3];
	m_init_identity(dcm_bg);

	/* Gyro data to be used in controller. */
	float gyr[3];
	gyr[0] = 0;
	gyr[1] = 0;
	gyr[2] = 0;

	/* Motor duty cycles */
	float motor_dc[4];

	while (TRUE) {
		counter++;
		time += CONTROL_DT*CH_FREQUENCY;

		update_ahrs(CONTROL_DT, dcm_bg, gyr);
		run_controller(throttle, dcm_bg, gyr, motor_dc, new_des_ang_pos);

		static uint8_t i, j;
		for (i=0; i<3; i++) {
			for (j=0; j<3; j++) {
				dbg_dcm[i][j] = dcm_bg[i][j];
			}
		}

		if (palReadPad(GPIOA, 10) == 1) {
			palSetPad(GPIOA, 7);
			for (i=0; i<4; i++) {
				motor_dc[i] = 0.0;   // TODO: put this in config.
			}
#if (NUM_ROTORS < 4)
			motor_dc[3] = 0.5;
#endif

#if (NUM_ROTORS < 3)
			motor_dc[2] = 0.5;
#endif
		}
		else {
			palClearPad(GPIOA, 7);
		}

		update_motors(motor_dc);

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

	setup_controller();

	palSetPadMode(GPIOA, 6, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOA, 7, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT_PULLUP);
	palClearPad(GPIOA, 6);
	palClearPad(GPIOA, 7);

	/*
	 * Short delay to let the various setup functions finish.
	 */
	chThdSleepMilliseconds(50);

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

