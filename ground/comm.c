#include <osuar_protocol.h>
#include <osuar_ringbuffer.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/time.h>

void tx(void)
{
	printf("TX\n");
}

void rx(osuar_rb_t *buf)
{
	printf("RX\n");
}

int main(int argc, char **argv)
{
	printf("OSUAR groundstation starting\n");

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
			rx(&rxbuf);
			timeradd(&tm_rx, &RX_DT, &tm_rx);
		}

		/* Don't peg the CPU. */
		usleep(1000);
	}

	return 0;
}

