#include <osuar_protocol.h>

#include <string.h>

uint32_t protocol_compute_crc(void *data, size_t data_size)
{
	(void) data;
	(void) data_size;

	return 0; // ish
}

/*
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
*/

size_t sizeoftype(uint8_t type)
{
	switch(type) {
	case UP_COMMAND_TYPE:
		return sizeof(up_command_t);
	case UP_CONFIG_TYPE:
		return sizeof(up_config_t);
	case DOWN_TELEM_HIGHFREQ_TYPE:
		return sizeof(down_telem_highfreq_t);
	case DOWN_TELEM_LOWFREQ_TYPE:
		return sizeof(down_telem_lowfreq_t);
	case DOWN_SYNC_TYPE:
		return sizeof(down_sync_t);
	default:
		return 0;
	}
}

/*
 * @brief Put packed message on UART transmit buffer.
 *
 * @param type Message type
 * @param message Pointer to message struct
 * @param txbuf Pointer to UART TX buffer
 *
 * @return Number of bytes added to TX buffer
 */
uint16_t send(uint8_t type, void *message, uint8_t *txbuf)
{
	static osuar_packet_t *packet;
	static int16_t msg_size_diff;
	static uint32_t crc;

	/* Determine size of payload from type, for example: */
	size_t message_size = sizeoftype(type);
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
	packet = (osuar_packet_t*) &txbuf[0];//&txbuf[open_index];

	//if (txbuf.available <= (sizeof(packet) + msg_size_diff)) {
	if (1) {
		packet->magic = MAGIC;
		packet->type = type;
		memcpy(&packet->message, message, message_size);   /* memcpy into ringbuffer */
		crc = protocol_compute_crc(packet, sizeof(packet->magic) + sizeof(packet->type) + message_size);
		memcpy(&packet->crc - msg_size_diff, &crc, sizeof(packet->crc));
		// TODO(yoos): Update buffer open_index
	}
	else {
		return 0;   // TODO(yoos)
	}

	return sizeof(osuar_packet_t) + msg_size_diff;
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
