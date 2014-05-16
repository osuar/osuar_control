/**
 * @file osuar_comm.h
 * @author Kyle Cesare
 * @brief Communication procedures.
 */

#ifndef OSUAR_COMM_H
#define OSUAR_COMM_H

#include <hal.h>

/*
 * Initialize things.
 */
void setup_comm(void);

void osuar_comm_data_received(BaseSequentialStream *sd);
bool osuar_comm_parse_input(float *throttle, float control_values[3]);

/*
 * Utility function to clear (i.e., zero) a buffer.
 */
void clear_buffer(uint8_t *buffer);

#endif /* OSUAR_COMM_H */

