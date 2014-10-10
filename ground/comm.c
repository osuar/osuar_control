#include <osuar_protocol.h>
#include <osuar_ringbuffer.h>

#include <serial.h>
#include <joystick.h>

#include <assert.h>

down_telem_highfreq_t msg_telem_hf;

void tx(int ser)
{
	if (armed) {
		uint8_t txbuf[200];
		size_t pkt_size;
		protocol_pack(UP_COMMAND_TYPE, &msg_cmd, txbuf, &pkt_size);
		//printf("----------------------------\n");
		//printf("%d\n", msg_cmd.throttle);
		//for (int i=0; i<20; i++) {
		//	printf("%x ", txbuf[i]);
		//}
		//printf("\n");
		//printf("----------------------------\n");
		write(ser, txbuf, pkt_size);
	}
}

void rx(int ser, osuar_rb_t *buf)
{
	/* Read from serial device. */
	int serlen;
	uint8_t serbuf[100];   // TODO(syoo): magic number
	serlen = read(ser, serbuf, 100);
	if (serlen > 0) {
		assert(osuar_rb_add(buf, serlen, serbuf) == serlen);
	}

	// TODO(yoos): use protocol_unpack and display the data in a GUI or
	// something.

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

void *comm(void *param)
{
	/* Configure serial device. */
	int ser = *((int*) param);
	set_interface_attribs(ser, B38400, 0);   // No parity
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
	uint8_t _rxbuf[200];
	osuar_rb_init(&rxbuf, sizeof(_rxbuf), _rxbuf);

	while(1) {
		/* Update current time */
		gettimeofday(&tm_cur, NULL);

		/* TX */
		if (timercmp(&tm_cur, &tm_tx, >) != 0) {
			tx(ser);
			timeradd(&tm_tx, &TX_DT, &tm_tx);
		}

		/* RX */
		if (timercmp(&tm_cur, &tm_rx, >) != 0) {
			rx(ser, &rxbuf);
			timeradd(&tm_rx, &RX_DT, &tm_rx);
		}

		/* Print */
		uint8_t printbuf[10];
		int r;
		while ((r = osuar_rb_remove(&rxbuf, 10, printbuf))) {
			int i;
			for (i=0; i<r; i++) {
				//printf("%2x: %c\n", printbuf[i], printbuf[i]);
				printf("%c", printbuf[i]);
			}
		}

		/* Don't peg the CPU. */
		usleep(1000);
	}
}

