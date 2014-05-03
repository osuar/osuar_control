#ifndef OSUAR_AHRS_H
#define OSUAR_AHRS_H

#include <ch.h>

#define TRIM_ANGLE_X 0.0
#define TRIM_ANGLE_Y 0.0

#define ACC_WEIGHT 0.005   /* Accelerometer weight relative to gyro's weight of 1 */
#define ACC_SCALE_WEIGHT 2   /* Gradually drive accelerometer weight to zero. For example, a value of 5 here will drive the accelerometer weight to zero if the magnitude of the measured acceleration differs by more than 1/5 gravity. */

/**
 * @brief Set up the AHRS.
 */
void setup_ahrs(void);

/**
 * @brief Update the AHRS.
 * @param dt Time interval since last update.
 * @output dcm_out The orientation of the body relative to the world.
 * @output gyr_out Gyro data out for controller.
 */
void update_ahrs(float dt, float dcm_out[3][3], float gyr_out[3]);

/**
 * @brief Output debug string.
 * @output buffer Buffer to populate with debug string.
 */
void debug_ahrs(uint8_t *buffer);

#endif /* OSUAR_AHRS_H */

