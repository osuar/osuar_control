#ifndef OSUAR_PROTOCOL_H
#define OSUAR_PROTOCOL_H

#include <stdlib.h>
#include <stdint.h>

#define MAGIC 0x01010101

#define   UP_COMMAND_TYPE            0
#define   UP_CONFIG_TYPE             1
#define   DOWN_TELEM_HIGHFREQ_TYPE   2
#define   DOWN_TELEM_LOWFREQ_TYPE    3
#define   DOWN_SYNC_TYPE             4

struct osuar_msg_t {
  uint32_t magic;
  uint8_t type;
  uint16_t crc;
  void *payload;
};

struct up_command_t {
  uint8_t mode;   /* Control mode (rate or position) */
  uint8_t axes[3];   /* Desired axis values (rad/s in XYZ or rad in XY) */
  uint8_t throttle;
};   /* 5 bytes */

struct up_config_t {
  float trim[2];   /* XY trim, Euler angles */
};   /* 8 bytes */

struct down_telem_highfreq_t {
  float dcm[9];   /* DCM, we really should be using quaternions */
};   /* 36 bytes */

struct down_telem_lowfreq_t {
  float dc[4];   /* Motor/servo duty cycles */
};   /* 16 bytes */

struct down_sync_t {
  float gains[6];   /* Velocity and position PID gains */
  float trim[2];   /* XY trim */
};   /* 32 bytes */

uint16_t protocol_compute_crc(void *data);

void protocol_pack(int type, void *packet, size_t packet_size, void *data, size_t *data_size);
void *protocol_unpack(int *id);

#endif
