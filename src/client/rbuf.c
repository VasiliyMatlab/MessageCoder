/*
 * file:        rbuf.c
 * author:      VasiliyMatlab
 * version:     1.0
 * date:        08.05.2023
 * copyright:   Vasiliy (c) 2023
 */

#include <stddef.h>

#include "rbuf.h"

// Инициализация кольцевого буфера
int32_t rbuf_init(struct rbuf *rb) {
	if (rb == NULL)
		return -1;
	
	rb->head = 0;
	rb->tail = 0;
	rb->len  = 0;
	return 0;
}

// Очистка кольцевого буфера
int32_t rbuf_drop(struct rbuf *rb) {
	if (rb == NULL)
		return -1;
	
	rb->head = 0;
	rb->tail = 0;
	rb->len  = 0;
	return 0;
}

// Дамп кольцевого буфера
void rbuf_dump(struct rbuf *rb) {
	uint32_t i, bi;
	
	if (rb != NULL) {
		tprintf("rbuf: len %u\r\n", rb->len);
		
		for(i = 0, bi = rb->tail; i < rb->len; i++, bi = RBUF_NEXT(bi)) {
			tprintf("%02X ", rb->buf[bi]);
			if((i & 0x7) == 0x7)
				tprintf("\r\n");
		}
		
		if((i & 0xF) != 0x8)
			tprintf("\r\n");
	}
}

// Добавление данных в кольцевой буфер
uint32_t rbuf_write(struct rbuf *rb, const uint8_t *buf, uint32_t size) {
	uint32_t i;

	for (i = 0; i < size; i++) {		
		rb->buf[rb->head] = buf[i];
		rb->head = RBUF_NEXT(rb->head);
		if (rb->len < RBUF_SIZE) {
			rb->len++;
		} else {
			rb->tail = RBUF_NEXT(rb->tail);
		}
	}
	
	return i;
}

// Получение данных из кольцевого буфера
uint32_t rbuf_read(struct rbuf *rb, uint8_t *buf, uint32_t size) {
	uint32_t i, idx;
	for (i = 0, idx = rb->tail; (i < size) && (i < rb->len); i++, idx = RBUF_NEXT(idx)) {
		buf[i] = rb->buf[idx];
	}
	return i;
}

// Сдвинуть начало кольцевого буфера
uint32_t rbuf_shift(struct rbuf *rb, uint32_t count) {
	if (count > rb->len)
		return -1;
	
	rb->tail = (rb->tail + count) & (RBUF_SIZE - 1);
	rb->len -= count;
	return 0;
}

// Поиск байта в кольцевом буфере
int32_t rbuf_search(struct rbuf *rb, uint8_t byte) {
	uint32_t i, idx;
	
	for(i = rb->tail, idx = 0; idx < rb->len; i = RBUF_NEXT(i), idx++) {
		if (rb->buf[i] == byte) {
			return idx;
		}
	}

	return -1;
}

// Поиск байта в кольцевом буфере, начиная со смещения offset
int32_t rbuf_search_from(struct rbuf *rb, uint32_t offset, uint8_t byte) {
	uint32_t i, idx;

	for(i = (rb->tail + offset) & (RBUF_SIZE-1), idx = 0; idx < rb->len; i = RBUF_NEXT(i), idx++) {
		if (rb->buf[i] == byte) {
			return idx;
		}
	}

	return -1;
}

// Количество байт с данными в кольцевом буфере
uint32_t rbuf_get_size_used(struct rbuf *rb) {
	return rb->len;
}

// Количество пустых байт в кольцевом буфере
uint32_t rbuf_get_size_free(struct rbuf *rb) {
	return (sizeof(rb->buf) - rb->len);
}
