#include "osuar_protocol.h"

#include <stdio.h>
#include <string.h>

int main(void) {
  struct up_command_t packet;
  packet.mode = 5;
  packet.axes[0] = 1;
  packet.axes[1] = 2;
  packet.axes[2] = 3;
  packet.throttle = 50;

  uint8_t data[100];
  memset(data, 0, 100);

  size_t sz = 0;
  protocol_pack(UP_COMMAND_TYPE, &packet, sizeof(packet), data + 10, &sz);

  printf("packed- size: %lu\n", sz);

  uint8_t id;
  void *d = protocol_unpack(data, 100, &id);
  switch(id) {
    case UP_COMMAND_TYPE: {
      struct up_command_t *in = (struct up_command_t *) d;
      printf("THrottle: %d\n", in->throttle);
      break;
    }
  }

  printf("unpacked- id: %u\n", id);
}
