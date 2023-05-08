/**
 * \file rbuf.h
 * \author VasiliyMatlab
 * \brief Ring Buffer module
 * \version 1.0
 * \date 08.05.2023
 * \copyright Vasiliy (c) 2023
 */

#ifndef __RBUF_H__
#define __RBUF_H__


#include <stdint.h>

#define RBUF_SIZE		512 							///< Размер буфера
#define RBUF_NEXT(idx)	(((idx) + 1) & (RBUF_SIZE - 1))	///< Следующий индекс буфера

/// Кольцевой буфер
struct rbuf {
	volatile uint32_t head;		///< Индекс головы данных
	volatile uint32_t tail;		///< Индекс хвоста данных
	volatile uint32_t len;		///< Размер данных в буфере
	uint8_t  buf[RBUF_SIZE];	///< Данные в буфере
};

/**
 * \brief Функция инициализации кольцевого буфера
 * 
 * \param[in,out] rb Указатель на дескриптор кольцевого буфера
 * \return 0; в случае ошибки - отрицательный код
 */
int32_t rbuf_init(struct rbuf *rb);

/**
 * \brief Функция очистки кольцевого буфера
 * 
 * \param[in,out] rb Указатель на дескриптор кольцевого буфера
 * \return 0; в случае ошибки - отрицательный код
 */
int32_t rbuf_drop(struct rbuf *rb);

/**
 * \brief Функция дампа кольцевого буфера
 * 
 * \param[in] rb Указатель на дескриптор кольцевого буфера
 */
void rbuf_dump(struct rbuf *rb);

/**
 * \brief Функция записи данных в кольцевой буфер
 * 
 * \param[in,out] rb Указатель на дескриптор кольцевого буфера
 * \param[in] buf Указатель на буфер с данными
 * \param[in] size Количество байт для записи
 * \return Количество записанных байт
 */
uint32_t rbuf_write(struct rbuf *rb, const uint8_t *buf, uint32_t size);

/**
 * \brief Функция чтения данных из кольцевого буфера
 * 
 * \param[in] rb Указатель на дескриптор кольцевого буфера
 * \param[out] buf Указатель на буфер, куда будут записаны данные
 * \param[in] size Количество байт для считывания
 * \return Количество прочитанных байт
 */
uint32_t rbuf_read(struct rbuf *rb, uint8_t *buf, uint32_t size);

/**
 * \brief Функция сдвига хвоста кольцевого буфера
 * 
 * \param[in,out] rb Указатель на дескриптор кольцевого буфера
 * \param[in] count Количество байт, на которые сдвигается хвост кольцевого буфера
 * \return 0; в случае ошибки - отрицательный код
 */
uint32_t rbuf_shift(struct rbuf *rb, uint32_t count);

/**
 * \brief Функция поиска определенного байта в кольцевом буфере
 * 
 * \param[in] rb Указатель на дескриптор кольцевого буфера
 * \param[in] byte Байт, который необходимо найти
 * \return Индекс, под которым расположен необходимый байт;
 * -1 в случае отсутствия данного байта в кольцевом буфере
 */
int32_t rbuf_search(struct rbuf *rb, uint8_t byte);

/**
 * \brief Функция поиска определенного байта в кольцевом буфере, 
 * начиная со определенного смещения
 * 
 * \param[in] rb Указатель на дескриптор кольцевого буфера
 * \param[in] offset Смещение
 * \param[in] byte Байт, который необходимо найти
 * \return Индекс, под которым расположен необходимый байт;
 * -1 в случае отсутствия данного байта в кольцевом буфере
 */
int32_t rbuf_search_from(struct rbuf *rb, uint32_t offset, uint8_t byte);

/**
 * \brief Функция, возвращающая количество байт с данными
 * в кольцевом буфере
 * 
 * \param[in] rb Указатель на дескриптор кольцевого буфера
 * \return Количество байт с данными
 */
uint32_t rbuf_get_size_used(struct rbuf *rb);

/**
 * \brief Функция, возвращающая количество пустых байт
 * в кольцевом буфере
 * 
 * \param[in] rb Указатель на дескриптор кольцевого буфера
 * \return Количество пустых байт
 */
uint32_t rbuf_get_size_free(struct rbuf *rb);


#endif /* __RBUF_H__ */
