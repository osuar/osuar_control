/**
 * @file osuar_comm.h
 * @author Kyle Cesare
 * @brief Communication procedures.
 */

#ifndef OSUAR_COMM_H
#define OSUAR_COMM_H

#include <ch.h>
#include <hal.h>
#include <chsprintf.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

extern uint8_t remote_comm_rxbuf[4];
extern uint8_t remote_comm_txbuf[200];

void osuar_comm_data_received(UARTDriver *uartp);
bool osuar_comm_parse_input(float *throttle, float control_values[3]);

#endif // OSUAR_COMM_H
