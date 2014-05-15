#include <osuar_protocol.h>

#include <stdio.h>
#include <string.h>

void test_send(void)
{
	up_command_t message;
	message.mode = 5;
	message.axes[0] = 1;
	message.axes[1] = 2;
	message.axes[2] = 3;
	message.throttle = 50;

	size_t msg_sz = sizeoftype(UP_CONFIG_TYPE);
	printf("packed- size: %lu\n", msg_sz);

	uint8_t txbuf[1000];
	memset(txbuf, 0, sizeof(txbuf));
	uint16_t packet_sz = send(UP_COMMAND_TYPE, &message, txbuf);
	printf("Added %d bytes (message size %d) to txbuf:\n\n", packet_sz, msg_sz);
	uint16_t i;
	for (i=0; i<sizeof(txbuf); i++) {
		printf("%02x", txbuf[i]);
	}
	printf("\n\n");
}

void test_receive(void)
{
	uint8_t type;
	void *d = 0;//protocol_unpack(data, 100, &id);
	switch(type) {
		case UP_COMMAND_TYPE: {
			up_command_t *in = (up_command_t *) d;
			printf("Throttle: %d\n", in->throttle);
			break;
		}
	}

	printf("unpacked- id: %u\n", type);
}


int main(void)
{
	test_send();
	test_receive();

	return 0;
}
