/**
 * @file osuar_controller.c
 * @author Soo-Hyun Yoo
 * @brief Cascading PD controllers.
 */

#include <osuar_controller.h>

/* TODO: Make setter function. */
static float ang_pos_xy_cap, ang_vel_xy_cap, ang_vel_z_cap;   // These aren't constants because we may want to change them mid-flight.
static pid_data_t pid_data[6];
static float dc_shift[3];
static float des_ang_pos[3], cur_ang_pos[3], des_ang_vel[3];
static float dbg_dc[4];

void angular_position_controller(float* cur_pos, float* cur_vel, float* des_pos, float* des_vel)
{
	/* Cap des_pos for X and Y axes. */
	/* TODO: Take cur_vel into account. */
	(void)cur_vel;

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

void angular_velocity_controller(float* cur_vel, float* des_vel, float* dc_shift)
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

void calculate_dc (float dc_throttle, float* dc_shift, float* dc_final)
{
#if (NUM_ROTORS == 2)
	dc_final[0] = dc_throttle + -dc_shift[0];
	dc_final[1] = dc_throttle +  dc_shift[0];
	dc_final[2] = 1 - (0.5 + dc_shift[1] + dc_shift[2]);
	dc_final[3] =      0.5 + dc_shift[1] - dc_shift[2];
#endif // NUM_ROTORS == 2

#if (NUM_ROTORS == 3)
	dc_final[I_ST] = 0.5 + dc_shift[2];
	dc_final[I_RT] = (dc_throttle +  dc_shift[1]) / cos((dc_final[I_ST] - 0.5) * M_PI);
	dc_final[I_RR] =  dc_throttle + -dc_shift[1] - dc_shift[0]*sqrt(3);
	dc_final[I_RL] =  dc_throttle + -dc_shift[1] + dc_shift[0]*sqrt(3);
#endif // NUM_ROTORS == 3

#if (NUM_ROTORS == 4)
	dc_final[0] = dc_throttle - dc_shift[1] - dc_shift[2];
	dc_final[1] = dc_throttle + dc_shift[0] + dc_shift[2];
	dc_final[2] = dc_throttle + dc_shift[1] - dc_shift[2];
	dc_final[3] = dc_throttle - dc_shift[0] + dc_shift[2];
#endif // NUM_ROTORS == 4

	/* Map duty cycles. */
	map_to_bounds(dc_final, NUM_ROTORS, 0.0, THROTTLE_CAP, dc_final);
}

void setup_controller(void)
{
	static uint8_t i;

	// Some constants that aren't (because we may want to change them
	// mid-flight).
	ang_pos_xy_cap = M_PI/4;
	ang_vel_xy_cap = 2*M_PI;
	ang_vel_z_cap  = M_PI/2;

	for (i=0; i<3; i++) {
		pid_data[I_ANG_POS_X+i].Kp = ANG_POS_KP;
		pid_data[I_ANG_POS_X+i].Ki = ANG_POS_KI;
		pid_data[I_ANG_POS_X+i].Kd = ANG_POS_KD;
		pid_data[I_ANG_POS_X+i].dt = CONTROL_DT;
		pid_data[I_ANG_POS_X+i].last_val = 0.0;
		pid_data[I_ANG_VEL_X+i].Kp = ANG_VEL_KP;
		pid_data[I_ANG_VEL_X+i].Ki = ANG_VEL_KI;
		pid_data[I_ANG_VEL_X+i].Kd = ANG_VEL_KD;
		pid_data[I_ANG_VEL_X+i].dt = CONTROL_DT;
		pid_data[I_ANG_VEL_X+i].last_val = 0.0;
	}
}

void run_controller(float throttle, float dcm_bg[3][3], float gyr[3], float dc[4])
{
	uint8_t i;
	// TODO: Calculate target rotation vector based on groundstation input and
	// scale to maximum rotation of ang_pos_xy_cap.
	des_ang_pos[0] = 0; //-joy.axes[SY] * ang_pos_xy_cap;
	des_ang_pos[1] = 0; // joy.axes[SX] * ang_pos_xy_cap;
	des_ang_pos[2] = 0; // joy.axes[SZ] * ang_pos_z_cap * CONTROL_LOOP_INTERVAL * MASTER_DT/1000000;

	// Calculate current rotation vector (Euler angles) from DCM and make
	// appropriate modifications to make PID calculations work later.
	cur_ang_pos[0] = -arctan2(dcm_bg[2][1], dcm_bg[2][2]) * dcm_bg[0][0] +
		              arctan2(dcm_bg[2][0], dcm_bg[2][2]) * dcm_bg[0][1];
	cur_ang_pos[1] =  arctan2(dcm_bg[2][0], dcm_bg[2][2]) * dcm_bg[1][1] -
		              arctan2(dcm_bg[2][1], dcm_bg[2][2]) * dcm_bg[1][0];

	// Keep abs(target - current) within [-PI, PI]. This way, nothing bad
	// happens as we rotate to any angle in [-PI, PI].
	for (i=0; i<2; i++){
		if(des_ang_pos[i] - cur_ang_pos[i] > M_PI){
			cur_ang_pos[i] += 2*M_PI;
		}
		else if (des_ang_pos[i] - cur_ang_pos[i] < -M_PI) {
			cur_ang_pos[i] -= 2*M_PI;
		}
	}

	// The magic happens here.
	angular_position_controller(cur_ang_pos, gyr, des_ang_pos, des_ang_vel);
	angular_velocity_controller(gyr, des_ang_vel, dc_shift);

	// Increase throttle to compensate for tilt, but only so much, and
	// definitely not when we're upside-down.
	throttle = (dcm_bg[2][2] > 0) ? throttle / MAX(dcm_bg[2][2], 0.707107) : 0;

	// Finally calculate motor duty cycles.
	calculate_dc(throttle, dc_shift, dc);

	// Debug
	for (i=0; i<4; i++) {
		dbg_dc[i] = dc[i];
	}
}

void map_to_bounds(float* input, uint8_t input_size, float bound_lower, float bound_upper, float* output)
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

void debug_controller(uint8_t *buffer)
{
	chsprintf(buffer, "%8u DC(%4u %4u %4u %4u) %7d %7d\r\n",
			chTimeNow(),
			(uint16_t) (dbg_dc[0]*1000),
			(uint16_t) (dbg_dc[1]*1000),
			(uint16_t) (dbg_dc[2]*1000),
			(uint16_t) (dbg_dc[3]*1000),
			(int32_t) (cur_ang_pos[0]*1000000),
			(int32_t) (cur_ang_pos[1]*1000000));
}

