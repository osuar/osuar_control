#include <osuar_protocol.h>
#include <osuar_ringbuffer.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>


down_telem_highfreq_t msg_telem_hf;

void tx(void)
{
}

void rx(int ser, osuar_rb_t *buf)
{
	/* Read from serial device. */
	int serlen;
	uint8_t serbuf[50];
	serlen = read(ser, serbuf, 1);
	osuar_rb_add(buf, serbuf, serlen);

	//uint8_t r = 1;
	//uint8_t msg_type;
	//uint8_t msg[MSG_SIZE_MAX];

	//while (r != 0) {
	//	r = protocol_get_message(buf, &msg_type, msg);
	//	if (r) {
	//		switch(msg_type) {
	//		case(DOWN_TELEM_HIGHFREQ_TYPE):
	//			memcpy(&msg_telem_hf, msg, sizeoftype(DOWN_TELEM_HIGHFREQ_TYPE));
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//}
}

int main(int argc, char **argv)
{
	printf("OSUAR groundstation starting\n");

	/* Usage */
	if (argc < 2) {
		printf("Usage:\n  %s <serial device>\n", argv[0]);
		exit(1);
	}

	/* Open serial device. */
	char *portname = argv[1];
	int ser = open(portname, O_RDWR);
	if (ser < 0) {
		printf("error %d opening %s: %s\n", errno, portname, strerror (errno));
		return 1;
	}
	set_interface_attribs(ser, B57600, 0);   // No parity
	set_blocking(ser, 0);

	/* Timer stuff. NOTE: not monotonic. */
	static struct timeval tm_cur, tm_rx, tm_tx;
	static struct timeval TX_DT = {0, 20000};   // 50 Hz
	static struct timeval RX_DT = {0, 10000};   // 100 Hz poll
	gettimeofday(&tm_cur, NULL);
	timeradd(&tm_cur, &TX_DT, &tm_tx);
	timeradd(&tm_cur, &RX_DT, &tm_rx);

	/* Ring buffer */
	osuar_rb_t rxbuf;
	osuar_rb_init(&rxbuf, 50);

	while(1) {
		/* Update current time */
		gettimeofday(&tm_cur, NULL);

		/* TX */
		if (timercmp(&tm_cur, &tm_tx, >) != 0) {
			tx();
			timeradd(&tm_tx, &TX_DT, &tm_tx);
		}

		/* RX */
		if (timercmp(&tm_cur, &tm_rx, >) != 0) {
			rx(ser, &rxbuf);

			timeradd(&tm_rx, &RX_DT, &tm_rx);
		}

		uint8_t printbuf[10];
		while (osuar_rb_remove(&rxbuf, printbuf, 1)) {
			printf("%2x: %c\n", printbuf[0], printbuf[0]);
			//printf("%c", printbuf[0]);
		}

		/* Don't peg the CPU. */
		usleep(1000);
	}

	return 0;
}

