#include <osuar_protocol.h>

uint16_t protocol_compute_crc(void *data) {
  return 0; // ish
}

void protocol_pack(int type, void *packet, size_t packet_size, void *data, size_t *data_size) {
  struct osuar_msg_t msg = {
    .magic = MAGIC,
    .type  = type,
    .payload = packet,
    .crc = protocol_compute_crc(packet)
  };

  data = &msg;
  *data_size = sizeof(msg) + packet_size;
}

void *protocol_unpack(int *id) {
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
