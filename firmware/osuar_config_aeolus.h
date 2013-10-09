#define NUM_ROTORS 4
#define ESC_COMM PWM

#define THROTTLE_CAP 1.0

/* Controller gains */
#define ANG_POS_KP 1.0
#define ANG_POS_KI 0.0
#define ANG_POS_KD 0.0
#define ANG_VEL_KP 0.2
#define ANG_VEL_KI 0.0
#define ANG_VEL_KD 0.0

/* IMU offsets */
#define GYR_X_OFFSET  0.001150
#define GYR_Y_OFFSET -0.016400
#define GYR_Z_OFFSET -0.003040
#define ACC_X_OFFSET -0.057000
#define ACC_Y_OFFSET -0.009000
#define ACC_Z_OFFSET -0.292000

/* PWM duty cycle constants */
#define MOTOR_PWM_DISABLED 0.50
#define MOTOR_PWM_MIN 0.55
#define MOTOR_PWM_MAX 0.99

