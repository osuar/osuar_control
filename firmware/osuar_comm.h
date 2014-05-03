/**
 * @file osuar_comm.h
 * @author Kyle Cesare
 * @brief Communication procedures.
 */

#ifndef OSUAR_COMM_H
#define OSUAR_COMM_H

#include <hal.h>

extern uint8_t remote_comm_rxbuf[4];
extern uint8_t remote_comm_txbuf[4];

void osuar_comm_data_received(UARTDriver *uartp);
bool osuar_comm_parse_input(float *throttle, float control_values[3]);

#endif // OSUAR_COMM_H
