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
void osuar_rb_init(osuar_rb_t *buf, size_t size);

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
 * @param buf Buffer
 * @param num_bytes Number of bytes to add
 * @param input Buffer to add
 *
 * @return Number of bytes actually added
 */
uint16_t osuar_rb_add(osuar_rb_t *buf, uint16_t num_bytes, uint8_t *input);

/*
 * Remover
 *
 * @param buf Buffer
 * @param num_bytes Number of bytes to remove
 *
 * @return Number of bytes actually removed
 */
uint16_t osuar_rb_remove(osuar_rb_t *buf, uint16_t num_bytes, uint8_t *output);

#endif /* OSUAR_RINGBUFFER_H */

