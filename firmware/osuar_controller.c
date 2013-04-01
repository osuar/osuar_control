/**
 * @file osuar_controller.c
 * @author Soo-Hyun Yoo
 * @brief Cascading PD controllers.
 */

#include <osuar_controller.h>

void angular_position_controller (float* cur_pos, float* cur_vel, float* des_pos, float* des_vel)
{
	/* Cap des_pos for X and Y axes. */
	int i;
	for (i=0; i<2; i++) {
		if (des_pos[i] > ang_pos_xy_cap) {
			des_pos[i] = ang_pos_xy_cap;
		}
		else if (des_pos[i] < -ang_pos_xy_cap) {
			des_pos[i] = -ang_pos_xy_cap;
		}
	}

	/* Calculate desired velocities. */
	desired_vel[0] = step_pid(cur_pos[0], des_pos[0], pid_data[PID_ANG_POS_X]);
	desired_vel[1] = step_pid(cur_pos[1], des_pos[1], pid_data[PID_ANG_POS_Y]);
	desired_vel[2] = step_pid(cur_pos[2], des_pos[2], pid_data[PID_ANG_POS_Z]);
}

void angular_velocity_controller (float* cur_vel, float* des_vel, float* dc_shift)
{
	/* Cap des_vel for X and Y axes. */
	int i;
	for (i=0; i<2; i++) {
		if (des_vel[i] > ang_vel_xy_cap) {
			des_vel[i] = ang_vel_xy_cap;
		}
		else if (des_vel[i] < -ang_vel_xy_cap) {
			des_vel[i] = -ang_vel_xy_cap;
		}
	}

	/* Cap des_vel for for Z axis. The desired velocity cap should be greater
	 * than the cap on the rate of change of the desired position commanded by
	 * the pilot.
	 */
	if (des_vel[2] > ang_vel_z_cap) {
		des_vel[2] = ang_vel_z_cap;
	}
	else if (des_vel[2] < -ang_vel_z_cap) {
		des_vel[2] = -ang_vel_z_cap;
	}

	/* Calculate duty cycle shifts. */
	dc_shift[0] = step_pid(cur_vel[0], des_vel[0], pid_data[PID_ANG_VEL_X]);
	dc_shift[1] = step_pid(cur_vel[1], des_vel[1], pid_data[PID_ANG_VEL_Y]);
	dc_shift[2] = step_pid(cur_vel[2], des_vel[2], pid_data[PID_ANG_VEL_Z]);
}

