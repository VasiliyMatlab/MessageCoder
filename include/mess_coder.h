/**
 * \file mess_coder.h
 * \author VasiliyMatlab
 * \brief Message Coder module
 * \version 1.0
 * \date 27.04.2023
 * \copyright Vasiliy (c) 2023
 */

#ifndef __MESS_CODER_H__
#define __MESS_CODER_H__


#include <stdint.h>

#define MESS_CODER_START_B			0xAB	///< Символ начала посылки (в закодированном потоке)
#define MESS_CODER_END_B			0xCD	///< Символ конца посылки (в закодированном потоке)

#define MESS_CODER_ENC_START		0x1E	///< Признак начала спец последовательности, после которого идет код спец последовательности (в закодированном потоке)
#define MESS_CODER_ENC_START_B		0x01	///< Код совпадения с началом посылки (в закодированном потоке)
#define MESS_CODER_ENC_DATA_B		0x02	///< Код совпадения с началом спец последовательности (в закодированном потоке)
#define MESS_CODER_ENC_END_B		0x03	///< Код совпадения с концом посылки (в закодированном потоке)

#define MESS_CODER_RC_ERROR			-1   	///< Общая ошибка
#define MESS_CODER_RC_NO_START		-21  	///< Символ начала посылки не найден
#define MESS_CODER_RC_NO_END		-22  	///< Символ конца посылки не найден
#define MESS_CODER_RC_OVERFLOW		-23  	///< Нехватка места в выходном буфере
#define MESS_CODER_RC_DECERR		-24  	///< Ошибка декодирования ключевой последовательности

/**
 * \brief Функция, преобразующая блок данных
 * в поток для передачи по последовательному интерфейсу;
 * добавляет символы начала и конца посылки
 * 
 * \param[out] out Указатель на выходной поток данных
 * \param[in] size_out Размер выходного потока данных
 * \param[in] in Указатель на входной блок данных
 * \param[in] size_in Размер входного блока данных
 * \return Положительный размер потока данных;
 * в случае ошибки - отрицательный код
 */
int messcoder_to_serial(void *out, uint32_t size_out,
			const void *in, uint32_t size_in);

/**
 * \brief Функция, преобразующая поток данных из последовательного интерфейса
 * в блок данных; убирает символы начала и конца посылки
 * 
 * \param[out] out Указатель на выходной блок данных
 * \param[in] size_out Размер выходного блока данных
 * \param[in] in Указатель на входной поток данных
 * \param[in] size_in Размер входного потока данных
 * \return Положительный размер блока данных;
 * в случае ошибки - отрицательный код
 */
int messcoder_from_serial(void *out, uint32_t size_out,
			const void *in, uint32_t size_in);

/**
 * \brief Функция, рассчитывающая размер буфера,
 * который необходим для закодированного потока
 * 
 * \param[in] in Указатель на входной блок данных
 * \param[in] size_in Размер входного блока данных
 * \return Размер буфера
 */
int messcoder_comp_enc_size(const void *in, uint32_t size_in);


#endif /* __MESS_CODER_H__ */
