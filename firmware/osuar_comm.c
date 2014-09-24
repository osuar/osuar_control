/**
 * @file osuar_comm.c
 * @author Kyle Cesare
 * @brief Communication procedures.
 */

#include <osuar_comm.h>
#include <osuar_protocol.h>
#include <osuar_config.h>
#include <string.h>

#define CONTROL_PACKET_SIZE 10
#define PACKET_BUFFER_SIZE (CONTROL_PACKET_SIZE * 2)

up_command_t g_cmd;

/*
 * SD1 config structure
 */
static SerialConfig sd1cfg = {
	UART1_BAUDRATE,
	0,
	0,
	0
};

/*
 * SD3 config structure
 */
static SerialConfig sd3cfg = {
	UART3_BAUDRATE,
	0,
	0,
	0
};

void setup_comm(void)
{
	/*
	 * Activate Serial driver on USART1 and USART3
	 * TODO(yoos): Enabling UART driver on USART2 interferes with I2C1. Need to
	 * check if Serial driver does the same. My guess is that it will.
	 */
	sdStart(&SD1, &sd1cfg);
	sdStart(&SD3, &sd3cfg);

	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(7));   // USART1 TX
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(7));   // USART1 RX
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));   // USART3 TX
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));   // USART3 RX
}

void osuar_comm_handle_receive(uint8_t *rxbuf, uint8_t num, osuar_rb_t *recv_rb)
{
	static uint8_t msg_type;   /* Received uplink message type */
	static uint8_t *msg;   /* Received message */
	static uint8_t r;

	/* Add received bytes to ring buffer. */
	osuar_rb_add(recv_rb, rxbuf, num);   /* TODO(yoos): handle failure */

	/* Try to parse message */
	r = protocol_get_message(recv_rb, &msg_type, msg);
	if (r) {
		switch(msg_type) {
		case(UP_COMMAND_TYPE):
			memcpy(&g_cmd, msg, sizeoftype(msg_type));
			break;
		case(UP_CONFIG_TYPE):
			/* TODO(yoos) */
			break;
		default:
			break;
		}
		g_cmd.mode = r;
	}
}

bool osuar_comm_parse_command(uint8_t mode, uint8_t throttle, uint8_t ang_pos[3])
{
	(void) mode;
	(void) throttle;
	(void) ang_pos;
	/* TODO: math */
	// Parse the command
	// TODO(cesarek): Restrict to 1 <= x <= 127 - 1 to prevent
	// header/trailer ambiguity?
	//ang_pos[0] = ((int8_t) packetbuf[(start_idx + 3) % PACKET_BUFFER_SIZE]) / 127.0;
	//ang_pos[1] = ((int8_t) packetbuf[(start_idx + 4) % PACKET_BUFFER_SIZE]) / 127.0;
	//ang_pos[2] = ((int8_t) packetbuf[(start_idx + 5) % PACKET_BUFFER_SIZE]) / 127.0;
	//*throttle = ((int8_t) packetbuf[(start_idx + 6) % PACKET_BUFFER_SIZE]) / 127.0;

    //    size_t i;
    //    for(i = 0; i < CONTROL_PACKET_SIZE; i++) {
    //      // TODO: drops packets when setting this to 0x00, why?
    //      packetbuf[(start_idx + i) % PACKET_BUFFER_SIZE] = 0x01;
    //    }

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

