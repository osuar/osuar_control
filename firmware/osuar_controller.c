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
	dc_final[0] = dc_throttle + dc_shift[0];
	dc_final[1] = dc_throttle - dc_shift[0];
	dc_final[2] =      0.5 + dc_shift[1] - dc_shift[2];
	dc_final[3] = 1 - (0.5 + dc_shift[1] + dc_shift[2]);
#endif // NUM_ROTORS == 2

#if (NUM_ROTORS == 3)
	dc_final[3] = 0.5 - dc_shift[2];
	dc_final[0] = dc_throttle + dc_shift[0]*0.866025 - 0.5*dc_shift[1];
	dc_final[1] = dc_throttle + dc_shift[1];
	dc_final[2] = dc_throttle - dc_shift[0]*0.866025 - 0.5*dc_shift[1];

	// TODO(yoos): Need to account for tail rotor tilt, but cosine doensn't
	// work right now.
	// dc_final[1] = (dc_throttle +  dc_shift[1]) / cosine((dc_final[3] - 0.5) * M_PI);
#endif // NUM_ROTORS == 3

#if (NUM_ROTORS == 4)
	dc_final[0] = dc_throttle - dc_shift[1] - dc_shift[2];
	dc_final[1] = dc_throttle + dc_shift[0] + dc_shift[2];
	dc_final[2] = dc_throttle + dc_shift[1] - dc_shift[2];
	dc_final[3] = dc_throttle - dc_shift[0] + dc_shift[2];
#endif // NUM_ROTORS == 4

	/* Map duty cycles. */
	map_to_bounds(dc_final, NUM_ROTORS, 0.0, MOTOR_DC_CAP, dc_final);
}

void setup_controller(void)
{
	static uint8_t i;

	// Some constants that aren't (because we may want to change them
	// mid-flight).
	ang_pos_xy_cap = M_PI/6;   /* May allocate button for position cap increase */
	ang_vel_xy_cap = 2*M_PI;   /* May allocate button for velocity cap increase */
	ang_vel_z_cap  = M_PI/2;   /* This will probably stay constant */

#if (NUM_ROTORS == 2)
	/* Motors */
	pid_data[I_ANG_POS_X].Kp = MOTOR_ANG_POS_KP;
	pid_data[I_ANG_POS_X].Ki = MOTOR_ANG_POS_KI;
	pid_data[I_ANG_POS_X].Kd = MOTOR_ANG_POS_KD;
	pid_data[I_ANG_VEL_X].Kp = MOTOR_ANG_VEL_KP;
	pid_data[I_ANG_VEL_X].Ki = MOTOR_ANG_VEL_KI;
	pid_data[I_ANG_VEL_X].Kd = MOTOR_ANG_VEL_KD;

	/* Servos */
	pid_data[I_ANG_POS_Y].Kp = SERVO_ANG_POS_KP;
	pid_data[I_ANG_POS_Y].Ki = SERVO_ANG_POS_KI;
	pid_data[I_ANG_POS_Y].Kd = SERVO_ANG_POS_KD;
	pid_data[I_ANG_VEL_Y].Kp = SERVO_ANG_VEL_KP;
	pid_data[I_ANG_VEL_Y].Ki = SERVO_ANG_VEL_KI;
	pid_data[I_ANG_VEL_Y].Kd = SERVO_ANG_VEL_KD;
	pid_data[I_ANG_POS_Z].Kp = SERVO_ANG_POS_KP;
	pid_data[I_ANG_POS_Z].Ki = SERVO_ANG_POS_KI;
	pid_data[I_ANG_POS_Z].Kd = SERVO_ANG_POS_KD;
	pid_data[I_ANG_VEL_Z].Kp = SERVO_ANG_VEL_KP;
	pid_data[I_ANG_VEL_Z].Ki = SERVO_ANG_VEL_KI;
	pid_data[I_ANG_VEL_Z].Kd = SERVO_ANG_VEL_KD;
#endif

#if (NUM_ROTORS == 3)
	/* Motors */
	pid_data[I_ANG_POS_X].Kp = MOTOR_ANG_POS_KP;
	pid_data[I_ANG_POS_X].Ki = MOTOR_ANG_POS_KI;
	pid_data[I_ANG_POS_X].Kd = MOTOR_ANG_POS_KD;
	pid_data[I_ANG_VEL_X].Kp = MOTOR_ANG_VEL_KP;
	pid_data[I_ANG_VEL_X].Ki = MOTOR_ANG_VEL_KI;
	pid_data[I_ANG_VEL_X].Kd = MOTOR_ANG_VEL_KD;

	/* Servos */
	pid_data[I_ANG_POS_Y].Kp = SERVO_ANG_POS_KP;
	pid_data[I_ANG_POS_Y].Ki = SERVO_ANG_POS_KI;
	pid_data[I_ANG_POS_Y].Kd = SERVO_ANG_POS_KD;
	pid_data[I_ANG_VEL_Y].Kp = SERVO_ANG_VEL_KP;
	pid_data[I_ANG_VEL_Y].Ki = SERVO_ANG_VEL_KI;
	pid_data[I_ANG_VEL_Y].Kd = SERVO_ANG_VEL_KD;
	pid_data[I_ANG_POS_Z].Kp = SERVO_ANG_POS_KP;
	pid_data[I_ANG_POS_Z].Ki = SERVO_ANG_POS_KI;
	pid_data[I_ANG_POS_Z].Kd = SERVO_ANG_POS_KD;
	pid_data[I_ANG_VEL_Z].Kp = SERVO_ANG_VEL_KP;
	pid_data[I_ANG_VEL_Z].Ki = SERVO_ANG_VEL_KI;
	pid_data[I_ANG_VEL_Z].Kd = SERVO_ANG_VEL_KD;
#endif

#if (NUM_ROTORS == 4)
	/* Motors */
	pid_data[I_ANG_POS_X].Kp = MOTOR_ANG_POS_KP;
	pid_data[I_ANG_POS_X].Ki = MOTOR_ANG_POS_KI;
	pid_data[I_ANG_POS_X].Kd = MOTOR_ANG_POS_KD;
	pid_data[I_ANG_VEL_X].Kp = MOTOR_ANG_VEL_KP;
	pid_data[I_ANG_VEL_X].Ki = MOTOR_ANG_VEL_KI;
	pid_data[I_ANG_VEL_X].Kd = MOTOR_ANG_VEL_KD;
	pid_data[I_ANG_POS_Y].Kp = MOTOR_ANG_POS_KP;
	pid_data[I_ANG_POS_Y].Ki = MOTOR_ANG_POS_KI;
	pid_data[I_ANG_POS_Y].Kd = MOTOR_ANG_POS_KD;
	pid_data[I_ANG_VEL_Y].Kp = MOTOR_ANG_VEL_KP;
	pid_data[I_ANG_VEL_Y].Ki = MOTOR_ANG_VEL_KI;
	pid_data[I_ANG_VEL_Y].Kd = MOTOR_ANG_VEL_KD;

	/* TODO: These should use different gains */
	pid_data[I_ANG_POS_Z].Kp = MOTOR_ANG_POS_KP;
	pid_data[I_ANG_POS_Z].Ki = MOTOR_ANG_POS_KI;
	pid_data[I_ANG_POS_Z].Kd = MOTOR_ANG_POS_KD;
	pid_data[I_ANG_VEL_Z].Kp = MOTOR_ANG_VEL_KP;
	pid_data[I_ANG_VEL_Z].Ki = MOTOR_ANG_VEL_KI;
	pid_data[I_ANG_VEL_Z].Kd = MOTOR_ANG_VEL_KD;
#endif

	for (i=0; i<3; i++) {
		pid_data[I_ANG_POS_X+i].dt = CONTROL_DT;
		pid_data[I_ANG_POS_X+i].last_val = 0.0;
		pid_data[I_ANG_VEL_X+i].dt = CONTROL_DT;
		pid_data[I_ANG_VEL_X+i].last_val = 0.0;
	}
}

void run_controller(float throttle, float dcm_bg[3][3], float gyr[3], float dc[4], float new_des_ang_pos[3])
{
	uint8_t i;
	// TODO: Calculate target rotation vector based on groundstation input and
	// scale to maximum rotation of ang_pos_xy_cap.
	des_ang_pos[0] = new_des_ang_pos[0] * ang_pos_xy_cap;
	des_ang_pos[1] = new_des_ang_pos[1] * ang_pos_xy_cap;
        // TODO(cesarek): handle yaw
	// des_ang_pos[2] = new_des_ang_pos[2] * ang_pos_z_cap * CONTROL_LOOP_INTERVAL * MASTER_DT/1000000;

	// Calculate current rotation vector (Euler angles) from DCM and make
	// appropriate modifications to make PID calculations work later.
	cur_ang_pos[0] = -arctan2(dcm_bg[2][1], dcm_bg[2][2]) * dcm_bg[0][0] +
		              arctan2(dcm_bg[2][0], dcm_bg[2][2]) * dcm_bg[0][1];
	cur_ang_pos[1] =  arctan2(dcm_bg[2][0], dcm_bg[2][2]) * dcm_bg[1][1] -
		              arctan2(dcm_bg[2][1], dcm_bg[2][2]) * dcm_bg[1][0];

	// Trim
	// TODO(yoos): Set trim to TRIM_ANGLE_X and _Y in config.
	cur_ang_pos[0] += 0.3;
	cur_ang_pos[1] += 0.3;

	// Keep abs(target - current) within [-PI, PI]. This way, nothing bad
	// happens as we rotate to any angle in [-PI, PI].
	for (i=0; i<3; i++){
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

	// Increase throttle to compensate for tilt up to 45 degrees. Cut throttle
	// at 60 degrees past vertical.
	// TODO: These numbers should be named sensibly and put in the config.
	throttle = (dcm_bg[2][2] > 0.5) ? throttle / MAX(dcm_bg[2][2], 0.707107) : 0;

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
	float offset = bound_lower - map_lower;
	float scale = (bound_upper - bound_lower) / (map_upper - map_lower);
	if (scale < 1) {
		for (i=0; i<input_size; i++) {
			output[i] = (input[i] - map_lower) * scale + bound_lower;
		}
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

