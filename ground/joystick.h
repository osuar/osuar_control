#include <fcntl.h>
#include <linux/joystick.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <osuar_protocol.h>

// TODO(yoos): dunno if I should preprocess joystick inputs at ground or onboard.
#define BUTTON_ZERO_THROTTLE       0
#define BUTTON_SOFT_ACRO_MODE      1
#define BUTTON_INC_ANG_POS_CAP     2
#define BUTTON_HARD_ACRO_MODE      3
#define BUTTON_RESET_YAW           4
#define BUTTON_ANG_POS_P_GAIN      5
#define BUTTON_ANG_VEL_P_GAIN      6
#define BUTTON_ANG_VEL_D_GAIN      7
#define BUTTON_Z_MOD               8   // Modify PPD gains for Z axis instead of XY.
#define BUTTON_UNDEFINED           9
#define BUTTON_UNDEFINED           10
#define BUTTON_INCREMENT_GAIN      11
#define BUTTON_DECREMENT_GAIN      12
#define BUTTON_UNDEFINED           13

up_command_t msg_cmd;

static bool armed = true;   // TODO(yoos): make false until throttle zero

static int16_t axes[3];
static uint16_t throttle = 3;
static int mode = 1;   // Velocity TODO(yoos): Need to import from FC code.
static int bf = 0x7fff;

void *joy(void *param);

