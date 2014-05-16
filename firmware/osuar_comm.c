/**
 * @file osuar_comm.c
 * @author Kyle Cesare
 * @brief Communication procedures.
 */

#include <osuar_comm.h>

#define CONTROL_PACKET_SIZE 10
#define PACKET_BUFFER_SIZE (CONTROL_PACKET_SIZE * 2)

uint8_t remote_comm_rxbuf[4];
uint8_t remote_comm_txbuf[4];

/* Circular buffer that continually takes data from the incoming rxbuf. */
static uint8_t packetbuf[PACKET_BUFFER_SIZE];

/* Current location of the write head in the buffer. */
static size_t write_idx;

void setup_comm(void)
{
	/*
	 * Activate Serial driver on USART1 and USART3
	 * TODO(yoos): Enabling UART driver on USART2 interferes with I2C1. Need to
	 * check if Serial driver does the same. My guess is that it will.
	 */
	sdStart(&SD1, NULL);
	sdStart(&SD3, NULL);

	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(7));   // USART1 TX
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(7));   // USART1 RX
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));   // USART3 TX
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));   // USART3 RX
}

void osuar_comm_data_received(BaseSequentialStream *sd)
{
	(void) sd;

	size_t i;
        for(i = 0; i < sizeof(remote_comm_rxbuf); i++) {
		packetbuf[write_idx] = remote_comm_rxbuf[i];
		write_idx = (write_idx + 1) % PACKET_BUFFER_SIZE;
        }

	// TODO(cesarek): warn if write_idx overlaps read_idx?

        // UART driver goes into idle mode when the buffer is filled.
	//uartStartReceive(&UARTD3, sizeof(remote_comm_rxbuf), remote_comm_rxbuf);
}

bool osuar_comm_parse_input(float *throttle, float ang_pos[3])
{
	// TODO(cesarek): Figure out a header/trailer that can't happen in
	// normal operation

	// Find the first starting index of the first packet
	size_t start_idx;
	for(start_idx = 0; start_idx < sizeof(packetbuf); start_idx++) {
		// Start magic sequence: 0xFF, 0xFF, 0xFF
		if(packetbuf[(start_idx + 0) % PACKET_BUFFER_SIZE] == 0xFF
				&& packetbuf[(start_idx + 1) % PACKET_BUFFER_SIZE] == 0xFF
				&& packetbuf[(start_idx + 2) % PACKET_BUFFER_SIZE] == 0xFF) {
			break;
		}
	}

	// Check if no header was found
	if(start_idx == sizeof(packetbuf)) {
		return false;
	}

	// Check the packet trailer sequence
	if(packetbuf[(start_idx + 7) % PACKET_BUFFER_SIZE] != 0x00
			|| packetbuf[(start_idx + 8) % PACKET_BUFFER_SIZE] != 0x00
			|| packetbuf[(start_idx + 9) % PACKET_BUFFER_SIZE] != 0x00) {
		return false;
	}

	// Parse the command
	// TODO(cesarek): Restrict to 1 <= x <= 127 - 1 to prevent
	// header/trailer ambiguity?
	ang_pos[0] = ((int8_t) packetbuf[(start_idx + 3) % PACKET_BUFFER_SIZE]) / 127.0;
	ang_pos[1] = ((int8_t) packetbuf[(start_idx + 4) % PACKET_BUFFER_SIZE]) / 127.0;
	ang_pos[2] = ((int8_t) packetbuf[(start_idx + 5) % PACKET_BUFFER_SIZE]) / 127.0;
	*throttle = ((int8_t) packetbuf[(start_idx + 6) % PACKET_BUFFER_SIZE]) / 127.0;

        size_t i;
        for(i = 0; i < CONTROL_PACKET_SIZE; i++) {
          // TODO: drops packets when setting this to 0x00, why?
          packetbuf[(start_idx + i) % PACKET_BUFFER_SIZE] = 0x01;
        }

	// TODO(cesarek): clear this portion of the buffer?

	return true;
}

void clear_buffer(uint8_t *buffer)
{
	uint16_t i;
	for (i=0; i<sizeof(buffer); i++) {
		buffer[i] = 0;
	}
}

