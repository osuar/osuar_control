#ifndef OSUAR_PROTOCOL_H
#define OSUAR_PROTOCOL_H

#include <osuar_ringbuffer.h>

#include <stdlib.h>
#include <stdint.h>

#define MAGIC 0x01010101   /* Header string. This should ideally never occur anywhere else in our serial stream. */
#define MSG_SIZE_MAX 100

#define UP_COMMAND_TYPE          10
#define UP_CONFIG_TYPE           1
#define DOWN_TELEM_HIGHFREQ_TYPE 2
#define DOWN_TELEM_LOWFREQ_TYPE  3
#define DOWN_SYNC_TYPE           4
#define DOWN_PLAINTEXT_TYPE      5

/*
 * So... this is weird, but it might work. CRC at end is only there to reserve
 * size. A single instance of this struct is statically instantiated in the
 * sender thread, and the CRC is actually tacked onto the end of the payload.
 *
 * TODO(yoos)
 */
typedef struct {
	uint32_t magic;   /* Header */
	uint8_t type;   /* Message type, indicates payload size. TODO(yoos): Need lookup. */
	uint8_t message[MSG_SIZE_MAX];   // TODO(cesarek): Don't want to define size, but have to...? Research flexible arrays/struct hack.
	uint32_t crc;   /* 32-bit CRC. We could probably get away with 16-bit, but the STM32F4 has a hardware CRC unit that is locked to 32-bit. */
} osuar_packet_t;


/* Uplink message types */

/* Control */
typedef struct {
	uint8_t mode;   /* Control mode (rate or position) */
	float axes[3];   /* Desired axis values (rad/s in XYZ or rad in XY) */
	float throttle;
} up_command_t;   /* 5 bytes */

/* Configuration */
typedef struct {
	float trim[2];   /* XY trim, Euler angles */
} up_config_t;   /* 8 bytes */


/* Downlink message types */

/* High-frequency telemetry */
typedef struct {
	float dcm[9];   /* DCM, we really should be using quaternions */
} down_telem_highfreq_t;   /* 36 bytes */

/* Low-frequency, possibly on-request, telemetry */
typedef struct {
	float dc[4];   /* Motor/servo duty cycles */
} down_telem_lowfreq_t;   /* 16 bytes */

/* Synchronization, on request only. Used to synchronize ground station with
 * vehicle on startup. */
typedef struct {
	float gains[6];   /* Velocity and position PID gains */
	float trim[2];   /* XY trim */
} down_sync_t;   /* 32 bytes */

/* Plaintext for debug */
typedef struct {
	uint8_t text[32];
} down_plaintext_t;   /* 32 bytes */


uint32_t protocol_compute_crc(void *data, size_t data_size);
size_t sizeoftype(uint8_t type);

/*
 * @brief Put packed message on UART transmit buffer.
 *
 * We assume txbuf has enough room to fit the largest possible packet.
 *
 * @param type Message type
 * @param message Pointer to message struct

 * @output txbuf Output buffer
 * @output packet_size Size of packet stuffed in output buffer
 */
void protocol_pack(uint8_t type, void *message, uint8_t *txbuf, size_t *packet_size);

/*
 * @brief Unpack packet.
 *
 * @param packet Packet
 * @param packet_size Size of packet
 *
 * @output 
 */
void *protocol_unpack(uint8_t *packet, size_t packet_size, uint8_t *type, void *message);

/*
 * @brief Find the first valid packet and its message type.
 *
 * @param buf Receive ringbuffer in which to search
 *
 * @output packet Packet struct to populate
 * @output type Message type
 *
 * @return 1 if valid packet found, 0 otherwise.
 */
uint8_t protocol_get_packet(osuar_rb_t *buf, osuar_packet_t *packet, uint8_t *type);

#endif

