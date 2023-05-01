/*
 * file:        mess_coder.c
 * author:      VasiliyMatlab
 * version:     1.0
 * date:        27.04.2023
 * copyright:   Vasiliy (c) 2023
 */

#include <stdio.h>

#include "mess_coder.h"

/**
 * \brief Кодирование данных
 * 
 * \param[out] out Указатель на закодированные данные
 * \param[in] size_out Ограничение по размеру на закодированные данные
 * \param[in] in Указатель на входные данные
 * \param[in] size_in Размер входных данных
 * \return Размер закодированных данных;
 * в случае ошибки - отрицательный код
 */
static int messcoder_encode(void *out, uint32_t size_out,
						 	const void *in, uint32_t size_in) {
	int rc;
	uint32_t idx_out = 0;
	uint8_t *istream = (uint8_t *) in;
	uint8_t *ostream = (uint8_t *) out;
	
	if (size_out == 0)
		return 0;
	
	// Добавляем байт начала
	ostream[idx_out++] = MESS_CODER_START_B;
	
	// Начинаем замену старт/стоп байтов; при этом в буфере должно 
	// оставаться достаточно места для вставки 2-х байт (замены)
	// или добавления байта окончания
	for (uint32_t idx_in = 0; (idx_in < size_in) && (idx_out < (size_out-1)); idx_in++) {
		switch (istream[idx_in]) {
		// Если встречаем символ начала посылки, то кодируем его
		case MESS_CODER_START_B:
			ostream[idx_out++] = MESS_CODER_ENC_START;		// спец символ
			ostream[idx_out++] = MESS_CODER_ENC_START_B;	// код начала посылки
			break;
		
		// Если встречаем спец символ, то кодируем его
		case MESS_CODER_ENC_START:
			ostream[idx_out++] = MESS_CODER_ENC_START;		// спец символ
			ostream[idx_out++] = MESS_CODER_ENC_DATA_B;		// код спец символа
			break;
		
		// Если встречаем символ конца посылки, то кодируем его
		case MESS_CODER_END_B:
			ostream[idx_out++] = MESS_CODER_ENC_START;		// спец символ
			ostream[idx_out++] = MESS_CODER_ENC_END_B;		// код конца посылки
			break;

		// Иначе оставляем символ как есть (без кодирования)
		default:
			ostream[idx_out++] = istream[idx_in];
			break;
		}
	}	
	
	// Добавляем байт окончания
	if (idx_out < size_out) {
		ostream[idx_out++] = MESS_CODER_END_B;
		rc = (int) idx_out;
	} else {
		// Переполнение выходного буфера
		rc = MESS_CODER_RC_OVERFLOW;
	}
	
	return rc;
}

/**
 * \brief Декодирование данных
 * 
 * \param[out] out Указатель на декодированные данные
 * \param[in] size_out Ограничение по размеру на декодированные данные
 * \param[in] in Указатель на входные данные
 * \param[in] size_in Размер входных данных
 * \return Размер декодированных данных;
 * в случае ошибки - отрицательный код
 */
static int messcoder_decode(void *out, uint32_t size_out,
			 			 	const void *in, uint32_t size_in) {
	int rc;
	uint32_t idx_in;
	uint32_t idx_out;
	uint8_t *istream = (uint8_t *) in;
	uint8_t *ostream = (uint8_t *) out;
	
	if(size_in == 0)
		return 0;
	
	// Ищем байт начала потока
	for (idx_in = 0; idx_in < size_in; idx_in++) {
		if (istream[idx_in] == MESS_CODER_START_B) {
			break;
		}
	}
	
	if (idx_in == size_in) {
		return MESS_CODER_RC_NO_START;
	}
	
	// Начинаем поиск последовательностей кодов и замену на исходные байты
	for (idx_out = 0; (idx_in < (size_in-1)) && (idx_out < size_out); idx_in++) {
		switch (istream[idx_in]) {
		// Нашли новое начало посылки;
		// сбрасываем счетчик выходного буфера и начинаем
		// писать заново
		case MESS_CODER_START_B:
			idx_out = 0;
			break;
		
		// Нашли конец посылки
		case MESS_CODER_END_B:
			goto check_end_byte;
		
		// Нашли байт начала кодовой последовательности;
		// расшифровываем следующий за ним байт для
		// восстановления исходной комбинации
		case MESS_CODER_ENC_START:
			switch (istream[idx_in + 1]) {
			// Следующий байт - код совпадения с началом посылки
			case MESS_CODER_ENC_START_B:
				ostream[idx_out++] = MESS_CODER_START_B;
				idx_in++;
				break;
			
			// Следующий байт - код совпадения со спец символом
			case MESS_CODER_ENC_DATA_B:
				ostream[idx_out++] = MESS_CODER_ENC_START;
				idx_in++;
				break;
			
			// Следующий байт - код совпадения с концом посылки
			case MESS_CODER_ENC_END_B:
				ostream[idx_out++] = MESS_CODER_END_B;
				idx_in++;
				break;
			
			// Неизвестная кодовая последовательность в
			// закодированном потоке (если мы это получили,
			// то это означает, что посылка была искажена
			// или кодировщик отправки потока отработал
			// неправильно)
			default:
				fprintf(stderr, "Error: MESS_CODER: invalid code 0x%2X\r\n",
						istream[idx_in+1]);
				return MESS_CODER_RC_DECERR;
			}
			break;
		
		// Нашли не закодированный байт
		default:
			ostream[idx_out++] = istream[idx_in];
			break;
		}
	}

check_end_byte:
	// Проверяем последний символ; поток должен завершаться
	// символом конца, иначе - ошибка
	if (istream[idx_in] != MESS_CODER_END_B) {
		if (idx_out >= size_out) {
			// Переполнение выходного буфера
			fprintf(stderr, "Error: MESS_CODER: output buffer overflow %u (avaliable %u)\r\n",
					idx_out+1, size_out);
			rc = MESS_CODER_RC_OVERFLOW;
		} else {
			// Нет символа окончания посылки
			rc = MESS_CODER_RC_NO_END;
		}
	} else {
		rc = (int) idx_out;
	}

	return rc;
}

// Преобразование блока данных в поток для передачи по последовательному интерфейсу
int messcoder_to_serial(void *out, uint32_t size_out,
		    		 	const void *in, uint32_t size_in) {
	if (!in || !size_in) {
		return MESS_CODER_RC_ERROR;
	}

	if (!out) {
		return MESS_CODER_RC_ERROR;
	}
	
	return messcoder_encode(out, size_out, in, size_in);
}

// Преобразование потока данных последовательного интерфейса в блок данных
int messcoder_from_serial(void *out, uint32_t size_out,
					   	  const void *in, uint32_t size_in) {
	if (!in || !size_in) {
		return MESS_CODER_RC_ERROR;
	}

	if (!out) {
		return MESS_CODER_RC_ERROR;
	}

	return messcoder_decode(out, size_out, in, size_in);
}

// Рассчитывание размера выходного буфера
int messcoder_comp_enc_size(const void *in, uint32_t size_in) {
	const uint8_t *istream = (const uint8_t *) in;
	uint32_t ostream_size = 2;
	
	for (uint32_t idx_in = 0; idx_in < size_in; idx_in++) {
		switch(istream[idx_in]) {
		case MESS_CODER_START_B:
		case MESS_CODER_END_B:
		case MESS_CODER_ENC_START:
			ostream_size += 2;
			break;
		default:
			ostream_size++;
			break;
		}
	}

	return (int) ostream_size;
}
