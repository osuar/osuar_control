/**
 * @file osuar_comm.h
 * @author Kyle Cesare
 * @brief Communication procedures.
 */

#ifndef OSUAR_COMM_H
#define OSUAR_COMM_H

#include <osuar_ringbuffer.h>
#include <osuar_protocol.h>
#include <hal.h>

/*
 * Global command struct
 *
 * TODO(yoos): This should maybe not be global.
 */
extern up_command_t g_cmd;

/*
 * Initialize things.
 */
void setup_comm(void);

/*
 * @brief Put received bytes in a ringbuffer for processing.
 *
 * @param rxbuf Small buffer storing latest few bytes off serial RX buffer.
 * @param num Number of bytes received.
 * @param recv_rb Ringbuffer in which to store received bytes.
 */
void osuar_comm_handle_receive(uint8_t *rxbuf, uint8_t num, osuar_rb_t *recv_rb);
bool osuar_comm_parse_command(uint8_t mode, uint8_t throttle, uint8_t ang_pos[3]);

/*
 * Utility function to clear (i.e., zero) a buffer.
 */
void clear_buffer(uint8_t *buffer);

#endif /* OSUAR_COMM_H */

