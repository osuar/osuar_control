#ifndef OSUAR_CONFIG_4_ROTOR_H
#define OSUAR_CONFIG_4_ROTOR_H

#define THROTTLE_CAP 0.7   // Maximum duty cycle allowed for any of the rotors.

/* Controller gains */
#define ANG_POS_KP 1.0
#define ANG_POS_KI 0.0
#define ANG_POS_KD 0.0
#define ANG_VEL_KP 0.2
#define ANG_VEL_KI 0.0
#define ANG_VEL_KD 0.0

// Rotor indices by angle from positive X. For example, I_R045 refers to the
// rotor at 45 degrees, i.e., front left.
#define I_R045 0
#define I_R135 1
#define I_R225 2
#define I_R315 3

#endif // OSUAR_CONFIG_4_ROTOR_H

