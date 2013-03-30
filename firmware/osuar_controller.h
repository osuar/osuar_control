/**
 * @file osuar_controller.h
 * @author Soo-Hyun Yoo
 * @brief Cascading PD controllers.
 */

#ifndef OSUAR_CONTROLLER_H
#define OSUAR_CONTROLLER_H

#include <osuar_config.h>
#include <osuar_pid.h>

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
void angular_position_controller (float* cur_pos, float* cur_vel, float* des_pos, float* des_vel);

#endif // OSUAR_CONTROLLER_H

