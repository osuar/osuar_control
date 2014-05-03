#ifndef MOTOR_H
#define MOTOR_H

/**
 * Set up pins, etc.
 */
void setup_motors(void);

/**
 * Send new desired motor speeds to ESCs.
 */
void update_motors(float dc[4]);

#endif // MOTOR_H

