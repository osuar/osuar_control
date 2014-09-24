#include <osuar_protocol.h>
#include <osuar_math.h>

#include <string.h>

uint32_t protocol_compute_crc(void *data, size_t data_size)
{
	(void) data;
	(void) data_size;

	return 0; // ish
}

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
	case DOWN_PLAINTEXT_TYPE:
		return sizeof(down_plaintext_t);
	default:
		return 0;
	}
}

void protocol_pack(uint8_t type, void *msg, uint8_t *txbuf, size_t *packet_size)
{
	static osuar_packet_t *packet;
	static size_t msg_size;
	static int16_t msg_size_diff;
	static uint32_t crc;

	/* Determine size of payload from type, for example: */
	msg_size = MIN(sizeoftype(type), MSG_SIZE_MAX);
	msg_size_diff = msg_size - MSG_SIZE_MAX;

	/*
	 * Stuff txbuf.
	 */
	packet = (osuar_packet_t*) txbuf;
	packet->magic = MAGIC;
	packet->type = type;
	memcpy(&packet->msg, msg, msg_size);   /* memcpy into ringbuffer */
	crc = protocol_compute_crc(packet, sizeof(packet->magic) + sizeoftype(packet->type) + msg_size);
	memcpy(&packet->crc - msg_size_diff, &crc, sizeof(packet->crc));

	/*
	 * Calculate packet size.
	 */
	*packet_size = sizeof(osuar_packet_t) + msg_size_diff;
}

/*
void protocol_unpack(uint8_t *rxbuf, size_t buffer_size, uint8_t *id)
{
	// Look for magic identifier to signify beginning of packet
	uint8_t i;
	for (i=0; i<buffer_size-sizeof(uint32_t); i++) {
		uint32_t *maybe_magic = (uint32_t *) (buffer + i);

		if(*maybe_magic == MAGIC) {
			// Found start

			// TODO(cesarek): Bounds checking
			osuar_msg_t *msg = (osuar_msg_t*) (buffer + i);
			*id = msg->type;

			return msg->payload;
		}
	}
}
*/

uint8_t protocol_get_message(osuar_rb_t *buf, uint8_t *type, uint8_t *msg)
{
	/* Some variables to help parse header */
	static uint32_t maybe_header = 0;
	static uint8_t tmp = 0;

	static uint32_t crc = 0;   /* TODO(yoos): LSB or MSB first? */

	while (buf->count > 0) {
		osuar_rb_remove(buf, &tmp, 1);
		maybe_header = (maybe_header << 8) + tmp;

		if (maybe_header == MAGIC && buf->count >= sizeoftype(*type) + 5) {
			osuar_rb_remove(buf, type, 1);
			osuar_rb_remove(buf, msg, sizeoftype(*type));
			osuar_rb_remove(buf, (uint8_t*) &crc, 4);
			/* TODO(yoos): Check CRC and return 0 if mismatch */
			return 1;
		}
	}

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
