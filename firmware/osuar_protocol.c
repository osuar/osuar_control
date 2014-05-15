#include <osuar_protocol.h>

#include <string.h>

uint32_t protocol_compute_crc(void *data, size_t data_size)
{
	(void) data;

	return 0; // ish
}

void protocol_pack(void *packet, void *message, uint8_t *message_type)
{
	static struct struct osuar_packet_t packet = {
		.magic = MAGIC,
		.type  = type,
	};

	memcpy(msg.payload, packet, packet_size);
	memcpy(data, &msg, sizeof(msg));

	*data_size = sizeof(msg) - 100 + packet_size;
}

void *protocol_unpack(uint8_t *buffer, size_t buffer_size, uint8_t *id)
{
	// Look for magic identifier to signify beginning of packet
	uint8_t i;
	for(i = 0; i < buffer_size - sizeof(uint32_t); i++) {
		uint32_t *maybe_magic = (uint32_t *) (buffer + i);

		if(*maybe_magic == MAGIC) {
			// Found start

			// TODO(cesarek): Bounds checking
			struct osuar_msg_t *msg = (struct osuar_msg_t *) (buffer + i);
			*id = msg->type;

			return msg->payload;
		}
	}

	return 0;
}

/*
 * @brief Put packed message on UART transmit buffer.
 *
 * @param type Message type
 * @param message Pointer to message struct
 * @param txbuf Pointer to UART TX buffer
 *
 * @return 0 on success, 1 otherwise, for now.   TODO(yoos)
 */
uint8_t send(uint8_t type, struct message_t *message, uint8_t *txbuf)
{
	static struct osuar_packet_t *packet;
	static int16_t msg_size_diff;
	static uint32_t crc;

	/* Determine size of payload from type, for example: */
	size_t message_size = sizeof(struct down_sync_t);
	msg_size_diff = message_size - MSG_SIZE_MAX;

	if (msg_size_diff > 0) {
		return 1;   /* This should never happen if we update MSG_SIZE_MAX appropriately after updating our message types. */
	}

	/*
	 * Check that txbuf has enough room. If so, treat the first open byte as
	 * a osuar_packet_t pointer to pass to the packer, which will then pack
	 * txbuf with payload as if it were a dedicated osuar_packet_t struct. Note
	 * the CRC is not necessarily stored at &packet.crc .
	 */
	packet = &txbuf[open_index];

	if (txbuf.available <= (sizeof(packet) + msg_size_diff)) {
		packet.magic = MAGIC;
		packet.type = type;
		memcpy(&packet.message, message, message_size);   /* memcpy into ringbuffer */
		crc = protocol_compute_crc(packet, sizeof(packet.magic) + sizeof(packet.type) + message_size);
		memcpy(&packet.crc - msg_size_diff, &crc, sizeof(packet.crc));
	}
	else {
		return 1;   // TODO(yoos)
	}

	/* On success, return 0. */
	return 0;
}

/*
int length;
void *data;
protocol_pack(UP_COMMAND_TYPE, my_packet, sizeof(my_packet), &data, &length);
*/

/*
send(REQ_MSG_TYPE, myreq);

int id;
void *data = read(&id);

switch(id) {
  case CONTROL_MSG_TYPE:
    struct osuar_control_msg_t mything = (osuar_control_msg_t) data;
    bla
  case: TELEM_MSG_TYPE:
    struct osuar_telem_msg_t mything = (osuar_telem_msg_t) data;
    bla
}
*/
