#include <osuar_ringbuffer.h>

void osuar_rb_init(osuar_rb_t *buf, size_t size)
{
	buf->size = size;
	buf->count = 0;
	buf->head = 0;
	uint8_t elems[size];
	buf->elems = elems;
}

void osuar_rb_destroy(osuar_rb_t *buf)
{
	free(buf->elems);
	free(buf);
}

uint8_t osuar_rb_add(osuar_rb_t *buf, uint8_t *input, uint16_t input_size)
{
	/* Check if buffer is too full. */
	if (buf->size < buf->count + input_size) {
		return 1;
	}

	/* Copy data. */
	static uint16_t i;
	static uint16_t tail;
	tail = (buf->head + buf->count) % buf->size;
	for (i=0; i<input_size; i++) {
		buf->elems[tail++] = input[i];
		if (tail == buf->size) tail = 0;
	}

	buf->count += input_size;

	return 0;
}

uint8_t osuar_rb_remove(osuar_rb_t *buf, uint8_t *output, uint16_t output_size)
{
	/* Check if buffer does not contain enough data. */
	if (buf->count < output_size) {
		return 1;
	}

	/* Copy data. */
	static uint16_t i;
	for (i=0; i<output_size; i++) {
		output[i] = buf->elems[buf->head++];
		if (buf->head == buf->size) buf->head = 0;
	}

	buf->count -= output_size;

	return 0;
}

