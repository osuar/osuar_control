#include <osuar_protocol.h>

#include <string.h>

uint16_t protocol_compute_crc(void *data) {
  (void) data;

  return 0; // ish
}

void protocol_pack(int type, void *packet, size_t packet_size, void *data, size_t *data_size) {
  struct osuar_msg_t msg = {
    .magic = MAGIC,
    .type  = type,
    .crc = protocol_compute_crc(packet)
  };

  memcpy(msg.payload, packet, packet_size);
  memcpy(data, &msg, sizeof(msg));

  *data_size = sizeof(msg) - 100 + packet_size;
}

void *protocol_unpack(uint8_t *buffer, size_t buffer_size, uint8_t *id) {
  // Look for magic identifier to signify beginning of packet
  for(uint8_t i = 0; i < buffer_size - sizeof(uint32_t); i++) {
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
