#ifndef MOTOR_H
#define MOTOR_H

#include <ch.h>
#include <hal.h>
#include <osuar_controller.h>
#include <osuar_config.h>

/**
 * Set up pins, etc.
 */
void setup_motors(void);

/**
 * Send new desired motor speeds to ESCs.
 */
void update_motors(float dc[4]);

#endif // MOTOR_H

