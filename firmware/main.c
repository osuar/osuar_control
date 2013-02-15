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
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(wa_led_thread, 128);
static msg_t led_thread(void *arg)
{
  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED3);       /* Orange.  */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED3);     /* Orange.  */
    chThdSleepMilliseconds(500);
  }
}

/*
 * Application entry point.
 */
int main(void) {

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

  while (TRUE) {
  }
}
