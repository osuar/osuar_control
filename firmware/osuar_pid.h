#ifndef OSUAR_PID_H
#define OSUAR_PID_H

/**
 * @brief PID parameters struct.
 */
typedef struct pid_data_t {
    uint8_t id;   // Used by calculate_pid().
    float Kp, Ki, Kd;   // Gains.
    float dt;   // Timestep In seconds.
    float last_val;
    float P, I, D;   // Terms.
};

/**
 * @brief Calculate PID output.
 *
 * @param cur Current value.
 * @param des Desired value.
 * @param pid_data PID parameters struct.
 */
float calculate_pid (float cur, float des, struct pid_data_t &pid_data);

#endif // OSUAR_PID_H

