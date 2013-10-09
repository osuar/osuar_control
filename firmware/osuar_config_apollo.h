#define NUM_ROTORS 4
#define ESC_COMM PWM

#define THROTTLE_CAP 0.7   // Maximum duty cycle allowed for any of the rotors.

/* Controller gains */
#define ANG_POS_KP 1.0
#define ANG_POS_KI 0.0
#define ANG_POS_KD 0.0
#define ANG_VEL_KP 0.2
#define ANG_VEL_KI 0.0
#define ANG_VEL_KD 0.0

/* IMU offsets */
#define GYR_X_OFFSET  0.029700
#define GYR_Y_OFFSET -0.015000
#define GYR_Z_OFFSET  0.031600
#define ACC_X_OFFSET -0.022030
#define ACC_Y_OFFSET -0.028988
#define ACC_Z_OFFSET -0.125280

