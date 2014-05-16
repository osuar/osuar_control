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
 * Constructor
 */
osuar_rb_t *osuar_rb_create(uint16_t size);

/*
 * Destructor
 */
void osuar_rb_destroy(osuar_rb_t *buf);

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
 * Getter
 *
 * @return 0 on success, 1 otherwise.
 */
uint8_t osuar_rb_get(osuar_rb_t *buf, uint8_t *output, uint16_t output_size);

#endif /* OSUAR_RINGBUFFER_H */

