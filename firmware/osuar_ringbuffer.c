#include <osuar_ringbuffer.h>

void osuar_rb_init(osuar_rb_t *buf, size_t size, uint8_t *elems)
{
	buf->size = size;
	buf->count = 0;
	buf->head = 0;
	buf->elems = elems;
}

uint16_t osuar_rb_add(osuar_rb_t *buf, uint16_t num_bytes, uint8_t *input)
{
	/* Check if buffer is too full. */
	if (buf->size - buf->count < num_bytes) {
		num_bytes = buf->size - buf->count;
	}

	/* Copy data. */
	static uint16_t i;
	static uint16_t tail;
	tail = (buf->head + buf->count) % buf->size;
	for (i=0; i<num_bytes; i++) {
		buf->elems[tail++] = input[i];
		if (tail == buf->size) tail = 0;
	}

	buf->count += num_bytes;

	return num_bytes;
}

uint16_t osuar_rb_remove(osuar_rb_t *buf, uint16_t num_bytes, uint8_t *output)
{
	/* Check if buffer does not contain enough data. */
	if (buf->count < num_bytes) {
		num_bytes = buf->count;
	}

	/* Copy data. */
	static uint16_t i;
	for (i=0; i<num_bytes; i++) {
		output[i] = buf->elems[buf->head++];
		if (buf->head == buf->size) buf->head = 0;
	}

	buf->count -= num_bytes;

	return num_bytes;
}

uint16_t osuar_rb_peek(osuar_rb_t *buf, uint16_t num_bytes, uint8_t *output)
{
	/* Check if buffer does not contain enough data. */
	if (buf->count < num_bytes) {
		num_bytes = buf->count;
	}

	/* Copy data. */
	static uint16_t i;
	uint16_t idx = buf->head;
	for (i=0; i<num_bytes; i++) {
		output[i] = buf->elems[idx++];
		if (idx == buf->size) idx = 0;
	}

	return num_bytes;
}

