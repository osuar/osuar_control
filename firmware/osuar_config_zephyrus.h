#define NUM_ROTORS 4
#define ESC_COMM PWM

#define MOTOR_DC_CAP 0.5

/* Controller gains */
#define MOTOR_ANG_POS_KP 1.0   /* TODO: tune these! */
#define MOTOR_ANG_POS_KI 0.0
#define MOTOR_ANG_POS_KD 0.0
#define MOTOR_ANG_VEL_KP 1.0
#define MOTOR_ANG_VEL_KI 0.0
#define MOTOR_ANG_VEL_KD 0.0

#define SERVO_ANG_POS_KP 0.5   /* TODO: tune these! */
#define SERVO_ANG_POS_KI 0.0
#define SERVO_ANG_POS_KD 0.0
#define SERVO_ANG_VEL_KP 0.05
#define SERVO_ANG_VEL_KI 0.0
#define SERVO_ANG_VEL_KD 0.0

/* IMU offsets */
#define GYR_X_OFFSET  0.037970
#define GYR_Y_OFFSET  0.007476
#define GYR_Z_OFFSET  0.005760
#define ACC_X_OFFSET -0.009000
#define ACC_Y_OFFSET  0.000000
#define ACC_Z_OFFSET  0.075000
#define GYR_X ( raw_gyr[0])
#define GYR_Y ( raw_gyr[1])
#define GYR_Z ( raw_gyr[2])
#define ACC_X ( raw_acc[0])
#define ACC_Y ( raw_acc[1])
#define ACC_Z ( raw_acc[2])

/* UART */
#define UART1_BAUDRATE 57600
#define UART3_BAUDRATE 38400

/* PWM duty cycle constants */
#define MOTOR_PWM_MIN 0.0   /* 1060/2000 us */
#define MOTOR_PWM_MAX 1.0   /* 1860/2000 us */

