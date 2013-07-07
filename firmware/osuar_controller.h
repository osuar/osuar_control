/**
 * @file osuar_controller.h
 * @author Soo-Hyun Yoo
 * @brief Cascading PD controllers.
 */

#ifndef OSUAR_CONTROLLER_H
#define OSUAR_CONTROLLER_H

#include <ch.h>
#include <hal.h>
#include <osuar_config.h>
#include <osuar_pid.h>
#include <osuar_math.h>

#define I_ANG_POS_X 0
#define I_ANG_POS_Y 1
#define I_ANG_POS_Z 2
#define I_ANG_VEL_X 3
#define I_ANG_VEL_Y 4
#define I_ANG_VEL_Z 5

/**
 * @brief Calculate desired angular velocities based on desired angular
 *     position inputs.
 *
 * @param cur_pos Current position.
 * @param cur_vel Current velocity. This is particularly used to avoid
 *     unnecessarily decelerating due to an extreme cur_pos when it would be
 *     easier to maintain cur_vel into the des_pos.
 * @param des_pos Desired position.
 *
 * @output des_vel Desired velocity.
 */
void angular_position_controller(float* cur_pos, float* cur_vel, float* des_pos, float* des_vel);

/**
 * @brief Calculate shifts in individual duty cycles of motors baseed on
 *     desired angular velocity inputs.
 *
 * @param cur_vel Current velocity.
 * @param des_vel Desired velocity.
 *
 * @output dc_shift Shift in duty cycle. This will be added to the throttle
 *     duty cycle value in calculate_pwm_duty_cycles().
 */
void angular_velocity_controller(float* cur_vel, float* des_vel, float* dc_shift);

/**
 * @brief Calculate PWM duty cycles.
 *
 * Note that all axes are based around the IMU. The 0th motor is in line with
 * the IMU's positive X axis, the 1st motor with the IMU's positive Y axis, and
 * so on.
 *
 * @param dc_throttle Throttle duty cycle. Range [0.0, 1.0]
 * @param dc_shift Shifts in duty cycles calculated by angular position and/or
 *     velocity controllers.
 *
 * @output dc_final Final duty cycles of motors and/or servos.
 */
void calculate_dc(float dc_throttle, float* dc_shift, float* dc_final);

/**
 * @brief Set up controller.
 */
void setup_controller(void);

/**
 * @brief Run the flight controller.
 *
 */
void run_controller(float throttle, float dcm_bg[3][3], float gyr[3], float dc[4]);

/**
 * @brief Map input array to be within desired bounds.
 *
 * @param input Input array of at least one element.
 * @param input_size Size of input array.
 * @param bound_lower Desired lower bound.
 * @param bound_upper Desired upper bound.
 *
 * @output output Mapped output array.
 */
void map_to_bounds(float* input, uint8_t input_size, float bound_lower, float bound_upper, float* output);

/**
 * debug
 */
void debug_controller(uint8_t *buffer);

#endif // OSUAR_CONTROLLER_H

