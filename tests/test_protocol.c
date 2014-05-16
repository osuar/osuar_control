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

	uint8_t txbuf[sizeof(osuar_packet_t)];
	memset(txbuf, 0, sizeof(txbuf));
	size_t msg_sz = sizeoftype(UP_CONFIG_TYPE);
	uint16_t packet_sz;

	protocol_pack(UP_CONFIG_TYPE, &message, txbuf, &packet_sz);

	printf("Added %d bytes to buffer (message size %d, buffer size %d):\n\n", packet_sz, msg_sz, sizeof(txbuf));

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
