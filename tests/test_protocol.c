#include <osuar_protocol.h>
#include <osuar_ringbuffer.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_send(void)
{
	up_command_t message;
	message.mode = 5;
	message.axes[0] = 1;
	message.axes[1] = 2;
	message.axes[2] = 3;
	message.throttle = 50;

	/* Pack a packet to send */
	uint8_t txbuf[sizeof(osuar_packet_t)];
	memset(txbuf, 0, sizeof(txbuf));
	size_t msg_sz = sizeoftype(UP_CONFIG_TYPE);
	size_t packet_sz;
	protocol_pack(UP_CONFIG_TYPE, &message, txbuf, &packet_sz);
	printf("Added %d bytes to buffer (message size %d, buffer size %d):\n\n", packet_sz, msg_sz, sizeof(txbuf));

	/* Dump content of txbuf */
	uint16_t i;
	for (i=0; i<sizeof(txbuf); i++) {
		printf("%02x", txbuf[i]);
	}
	printf("\n\n");
}

void test_receive(void)
{
	uint8_t type;
	/* Create ring buffer */
	osuar_rb_t *rxbuf = osuar_rb_create(50);

	assert(rxbuf->size == 50);
	assert(rxbuf->count == 0);
	assert(rxbuf->head == 0);

	/* Create incoming packets */
	down_sync_t msg_sync;
	msg_sync.gains[0] = 1;
	msg_sync.gains[1] = 2;
	msg_sync.gains[2] = 4.4;
	msg_sync.gains[3] = 8.8;
	msg_sync.gains[4] = 16;
	msg_sync.gains[5] = 32;
	msg_sync.trim[0] = -1.2;
	msg_sync.trim[1] = 1.5;

	/* The transmitter would send us this packet */
	type = DOWN_SYNC_TYPE;
	uint8_t txbuf[sizeof(osuar_packet_t)];
	memset(txbuf, 0, sizeof(txbuf));
	size_t msg_sz = sizeoftype(type);
	size_t packet_sz;
	uint16_t i;
	protocol_pack(type, &msg_sync, txbuf, &packet_sz);
	printf("Added %d bytes to TX buffer (message size %d, buffer size %d):\n\n", packet_sz, msg_sz, sizeof(txbuf));
	printf("Contents of txbuf:\n\n");
	for (i=0; i<sizeof(txbuf); i++) {
		printf("%02x", txbuf[i]);
	}
	printf("\n\n");

	/* Copy data to rxbuf */
	assert(osuar_rb_add(rxbuf, txbuf, packet_sz) == 0);
	assert(rxbuf->count == packet_sz);
	printf("Copied %d bytes from TX to RX buffer (buffer size %d).\n", packet_sz, rxbuf->size);
	printf("Contents of rxbuf:\n\n");
	for (i=0; i<rxbuf->size; i++) {
		printf("%02x", rxbuf->elems[i]);
	}
	printf("\n\n");

	/* Try to overfill buffer */
	assert(osuar_rb_add(rxbuf, txbuf, packet_sz) == 1);

	/* Pull data from RX buffer */
	uint8_t tmp[50];
	assert(osuar_rb_get(rxbuf, tmp, packet_sz-2) == 0);
	assert(rxbuf->head == packet_sz-2);
	assert(rxbuf->count == 2);

	/* This time, the buffer should have some room. */
	assert(osuar_rb_add(rxbuf, txbuf, packet_sz) == 0);
	assert(rxbuf->head == packet_sz-2);
	assert(rxbuf->count == packet_sz+2);

	/* Attempt to get more than available. */
	assert(osuar_rb_get(rxbuf, tmp, 30) == 0);
	assert(rxbuf->head == (packet_sz+28) % rxbuf->size);
	assert(rxbuf->count == (packet_sz-28) % rxbuf->size);
	assert(osuar_rb_get(rxbuf, tmp, 30) == 1);
	assert(rxbuf->head == (packet_sz+28) % rxbuf->size);
	assert(rxbuf->count == (packet_sz-28) % rxbuf->size);

	osuar_rb_destroy(rxbuf);
}


int main(void)
{
	printf("Testing receiver...\n");
	test_receive();
	printf("DONE\n\n");

	printf("Testing sender...\n");
	test_send();
	printf("DONE\n\n");

	return 0;
}
