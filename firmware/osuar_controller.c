/**
 * @file osuar_controller.c
 * @author Soo-Hyun Yoo
 * @brief Cascading PD controllers.
 */

#include <osuar_controller.h>

/* TODO: Make setter function. */
static float ang_pos_xy_cap, ang_vel_xy_cap, ang_vel_z_cap;
static pid_data_t pid_data[6];
static dc_final[8], target_ang_pos[3], curent_ang_pos[3];
float throttle;

void angular_position_controller (float* cur_pos, float* cur_vel, float* des_pos, float* des_vel)
{
	/* Cap des_pos for X and Y axes. */
	/* TODO: Take cur_vel into account. */
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
	des_vel[0] = calculate_pid(cur_pos[0], des_pos[0], &pid_data[I_ANG_POS_X]);
	des_vel[1] = calculate_pid(cur_pos[1], des_pos[1], &pid_data[I_ANG_POS_Y]);
	des_vel[2] = calculate_pid(cur_pos[2], des_pos[2], &pid_data[I_ANG_POS_Z]);
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
	dc_shift[0] = calculate_pid(cur_vel[0], des_vel[0], &pid_data[I_ANG_VEL_X]);
	dc_shift[1] = calculate_pid(cur_vel[1], des_vel[1], &pid_data[I_ANG_VEL_Y]);
	dc_shift[2] = calculate_pid(cur_vel[2], des_vel[2], &pid_data[I_ANG_VEL_Z]);
}

void calculate_duty_cycles (float dc_throttle, float* dc_shift, float* dc_final)
{
#if (NUM_ROTORS == 3)
	dc_final[I_ST] = 0.5 + dc_shift[2];
	dc_final[I_RT] = (dc_throttle +  dc_shift[1]) / cos((dc_final[I_ST] - 0.5) * M_PI);
	dc_final[I_RR] =  dc_throttle + -dc_shift[1] - dc_shift[0]*sqrt(3);
	dc_final[I_RL] =  dc_throttle + -dc_shift[1] + dc_shift[0]*sqrt(3);
#endif // NUM_ROTORS == 3

#if (NUM_ROTORS == 4)
        dc_final[I_R315] = dc_throttle - dc_shift[1] - dc_shift[0];
        dc_final[I_R045] = dc_throttle - dc_shift[1] + dc_shift[0];
        dc_final[I_R225] = dc_throttle + dc_shift[1] + dc_shift[0];
        dc_final[I_R135] = dc_throttle + dc_shift[1] - dc_shift[0];
#endif // NUM_ROTORS == 4

	/* Map duty cycles. */
	map_to_bounds(dc_final, NUM_ROTORS, 0.0, THROTTLE_CAP, dc_final);
}

// TODO: Flesh this out. Refer to the fly() function in https://github.com/yoos/tricopter/blob/master/src/pilot.cpp
void flight_controller (float* dc)
{
	// Calculate target rotation vector based on groundstation input and scale to maximum rotation of ang_pos_xy_cap.
	// TODO
        target_ang_pos[0] = -joy.axes[SY] * ang_pos_xy_cap;
        target_ang_pos[1] =  joy.axes[SX] * ang_pos_xy_cap;
        target_ang_pos[2] =  joy.axes[SZ] * ang_pos_z_cap * CONTROL_LOOP_INTERVAL * MASTER_DT/1000000;

	// Calculate current rotation vector (Euler angles) from DCM and make appropriate modifications to make PID calculations work later.
	// TODO
        current_ang_pos[0] = atan2(bodyDCM[2][1], bodyDCM[2][2]) * bodyDCM[0][0] + atan2(bodyDCM[2][0], bodyDCM[2][2]) * bodyDCM[0][1];
        current_ang_pos[1] = atan2(bodyDCM[2][0], dobyDCM[2][2]) * bodyDCM[1][1] - atan2(bodyDCM[2][1], bodyDCM[2][2]) * bodyDCM[1][0];

	// Keep abs(target - current) within [-PI, PI]. This way, nothing bad happens as we rotate to any angle in [-PI, PI].
	// TODO
        for(int i = 0; i < 3; i++){
            if(target_ang_pos[i] - current_ang_pos[i] > PI){
                current_ang_pos[i] += 2*PI;
            }
            else{
                current_ang_pos[i] -= 2*PI;
            }
        }

	angular_position_controller(cur_pos, cur_vel, des_pos, des_vel);

	angular_velocity_controller(cur_vel, des_vel, dc_shift);

	// Set throttle based on groundstation input.
	// TODO
        throttle = throttle_enabled * (0.8*joy.axis[ST1] + 0.2*joy.axes[ST0]) * (TMAX - TMIN);

	// Increase throttle based on tilt.
	// TODO
        throttle = throttle / MAX(bodyDCM[2][2], 0.707107);

	calculate_pwm_duty_cycles(throttle, dc_shift, dc);
}

void map_to_bounds (float* input, uint8_t input_size, float bound_lower, float bound_upper, float* output)
{
	float map_lower = bound_lower;
	float map_upper = bound_upper;

	/* Find maximum and minimum input values. */
	int i;
	for (i=0; i<input_size; i++) {
		if (map_upper < input[i]) {
			map_upper = input[i];
		}
		else if (map_lower > input[i]) {
			map_lower = input[i];
		}
	}

	/* Limit, but not fit, the inputs to the maximum and minimum values. */
	float offset = ABS(bound_lower - map_lower);
	float scale = 1.0 / (map_upper - map_lower);
	for (i=0; i<input_size; i++) {
		output[i] = (input[i] + offset) * scale;
	}
}
