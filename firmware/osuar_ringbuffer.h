#ifndef OSUAR_RINGBUFFER_H
#define OSUAR_RINGBUFFER_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
	uint16_t size;   /* I.e., capacity */
	uint16_t count;   /* Number of slots filled */
	uint16_t head;   /* Head */
	uint8_t *elems;   /* Elements */
} osuar_rb_t;

/*
 * Initializer
 */
void osuar_rb_init(osuar_rb_t *buf, uint8_t *elems, size_t size);

/*
 * Adder
 *
 * If there is not enough room in buffer to accomodate all data, no data will
 * be written.
 *
 * @return 0 on success, 1 otherwise.
 */
uint8_t osuar_rb_add(osuar_rb_t *buf, uint8_t *input, uint16_t input_size);

/*
 * Remover
 *
 * @return 0 on success, 1 otherwise.
 */
uint8_t osuar_rb_remove(osuar_rb_t *buf, uint8_t *output, uint16_t output_size);

#endif /* OSUAR_RINGBUFFER_H */
