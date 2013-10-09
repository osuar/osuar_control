#define NUM_ROTORS 2
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
#define GYR_X_OFFSET  0.001100
#define GYR_Y_OFFSET -0.012500
#define GYR_Z_OFFSET -0.013650
#define ACC_X_OFFSET -0.007365
#define ACC_Y_OFFSET -0.010660
#define ACC_Z_OFFSET -0.038730
#define GYR_X ( raw_gyr[0])
#define GYR_Y (-raw_gyr[1])
#define GYR_Z (-raw_gyr[2])
#define ACC_X ( raw_acc[0])
#define ACC_Y (-raw_acc[1])
#define ACC_Z (-raw_acc[2])

/* PWM duty cycle constants */
#define MOTOR_PWM_DISABLED 0.50
#define MOTOR_PWM_MIN 0.55
#define MOTOR_PWM_MAX 0.99
#define SERVO_PWM_MIN 0.0517
#define SERVO_PWM_MAX 0.0944

