#define NUM_ROTORS 4
#define ESC_COMM PWM

#define THROTTLE_CAP 0.7   // Maximum duty cycle allowed for any of the rotors.

/* Controller gains */
#define MOTOR_ANG_POS_KP 1.0
#define MOTOR_ANG_POS_KI 0.0
#define MOTOR_ANG_POS_KD 0.0
#define MOTOR_ANG_VEL_KP 0.2
#define MOTOR_ANG_VEL_KI 0.0
#define MOTOR_ANG_VEL_KD 0.0

/* IMU offsets */
#define GYR_X_OFFSET  0.029700
#define GYR_Y_OFFSET -0.015000
#define GYR_Z_OFFSET  0.031600
#define ACC_X_OFFSET  0.015830
#define ACC_Y_OFFSET -0.023488
#define ACC_Z_OFFSET  0.085280
#define GYR_X ( raw_gyr[0])
#define GYR_Y ( raw_gyr[1])
#define GYR_Z ( raw_gyr[2])
#define ACC_X ( raw_acc[0])
#define ACC_Y ( raw_acc[1])
#define ACC_Z ( raw_acc[2])

/* PWM duty cycle constants */
#define MOTOR_PWM_DISABLED 0.40
#define MOTOR_PWM_MIN 0.44
#define MOTOR_PWM_MAX 0.99

