#ifndef OSUAR_PROTOCOL_H
#define OSUAR_PROTOCOL_H

#include <ch.h>

#define MAGIC 0x01010101   /* Header string. This should ideally never occur anywhere else in our serial stream. */

#define UP_COMMAND_TYPE          10
#define UP_CONFIG_TYPE           1
#define DOWN_TELEM_HIGHFREQ_TYPE 2
#define DOWN_TELEM_LOWFREQ_TYPE  3
#define DOWN_SYNC_TYPE           4

struct osuar_msg_t {
	uint32_t magic;   /* Header */
	uint8_t type;   /* Message type, indicates payload size. TODO(yoos): Need lookup. */
	uint32_t crc;   /* 32-bit CRC. We could probably get away with 16-bit, but the STM32F4 has a hardware CRC unit that is locked to 32-bit. */
	uint8_t payload[100];   // TODO(cesarek): Don't want to define size, but have to...? Research flexible arrays/struct hack.
};


/* Uplink message types */

/* Control */
struct up_command_t {
	uint8_t mode;   /* Control mode (rate or position) */
	uint8_t axes[3];   /* Desired axis values (rad/s in XYZ or rad in XY) */
	uint8_t throttle;
};   /* 5 bytes */

/* Configuration */
struct up_config_t {
	float trim[2];   /* XY trim, Euler angles */
};   /* 8 bytes */


/* Downlink message types */

/* High-frequency telemetry */
struct down_telem_highfreq_t {
	float dcm[9];   /* DCM, we really should be using quaternions */
};   /* 36 bytes */

/* Low-frequency, possibly on-request, telemetry */
struct down_telem_lowfreq_t {
	float dc[4];   /* Motor/servo duty cycles */
};   /* 16 bytes */

/* Synchronization, on request only. Used to synchronize ground station with
 * vehicle on startup. */
struct down_sync_t {
	float gains[6];   /* Velocity and position PID gains */
	float trim[2];   /* XY trim */
};   /* 32 bytes */


uint32_t protocol_compute_crc(void *data);

void protocol_pack(int type, void *packet, size_t packet_size, void *data, size_t *data_size);
void *protocol_unpack(uint8_t *buffer, size_t buffer_size, uint8_t *id);

#endif

