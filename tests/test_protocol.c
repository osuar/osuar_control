#include <osuar_protocol.h>

#include <stdio.h>
#include <string.h>

int main(void) {
	up_command_t message;
	message.mode = 5;
	message.axes[0] = 1;
	message.axes[1] = 2;
	message.axes[2] = 3;
	message.throttle = 50;

	uint8_t data[100];
	memset(data, 0, 100);

	size_t sz = 0;
	//protocol_pack(UP_COMMAND_TYPE, &packet, sizeof(packet), data + 10, &sz);

	printf("packed- size: %lu\n", sz);

	uint8_t id;
	void *d = 0;//protocol_unpack(data, 100, &id);
	switch(id) {
		case UP_COMMAND_TYPE: {
			up_command_t *in = (up_command_t *) d;
			printf("THrottle: %d\n", in->throttle);
			break;
		}
	}

	printf("unpacked- id: %u\n", id);
}
