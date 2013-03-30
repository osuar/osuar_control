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
	desired_vel[1] = step_pid(cur_pos[1], des_pos[1], pid_data[PID_ANG_POS_X]);
	desired_vel[2] = step_pid(cur_pos[2], des_pos[2], pid_data[PID_ANG_POS_X]);
}

