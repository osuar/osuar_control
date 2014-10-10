#include <joystick.h>

void *joy(void *param)
{
	int *fd = (int *) param;
	uint16_t throttle_left = 0;
	uint16_t throttle_right = 0;

	struct js_event e;
	while (true) {
		read(*fd, &e, sizeof(e));

		if (e.type == JS_EVENT_AXIS) {
			// TODO: Pull axis configuration out

			// Check the axis
			if (e.number == 0 || e.number == 1 || e.number == 3) {
				if(e.number == 1) {
					e.value = -e.value;
				}

				// Ignore the real axix 2 (throttle 1) and make it yaw
				int n = e.number == 3 ? 2 : e.number;
				msg_cmd.axes[n] = e.value;
			}
			else if (e.number == 2) {
				throttle_left = -e.value + 0x7fff;
				msg_cmd.throttle = 0.2 * throttle_left + 0.8 * throttle_right;

				if(!armed && throttle < 10) {
					armed = true;
					printf("Arming!\n");
				}
			}
			else if (e.number == 4) {
				throttle_right = -e.value + 0x7fff;
				msg_cmd.throttle = 0.2 * throttle_left + 0.8 * throttle_right;
			}
		}
		else if (e.type == JS_EVENT_BUTTON) {
			msg_cmd.bf = ((0 << e.number) & msg_cmd.bf) + (e.value << e.number);
			switch (e.number) {
			case BUTTON_ZERO_THROTTLE:
				break;
			case BUTTON_SOFT_ACRO_MODE:
				break;
			case BUTTON_INC_ANG_POS_CAP:
				break;
			case BUTTON_HARD_ACRO_MODE:
				break;
			case BUTTON_RESET_YAW:
				break;
			case BUTTON_ANG_POS_P_GAIN:
				break;
			case BUTTON_ANG_VEL_P_GAIN:
				break;
			case BUTTON_ANG_VEL_D_GAIN:
				break;
			case BUTTON_Z_MOD:
				break;
			case BUTTON_INCREMENT_GAIN:
				break;
			case BUTTON_DECREMENT_GAIN:
				break;
			}
		}
	}
}


