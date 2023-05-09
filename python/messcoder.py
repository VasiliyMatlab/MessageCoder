#!/usr/bin/env python3

"""messcoder.py: Message Coder module"""

__author__ = "VasiliyMatlab"
__copyright__ = "Vasiliy (c) 2023"


MESS_CODER_START_B      = 0xAB  # Символ начала посылки (в закодированном потоке)
MESS_CODER_END_B        = 0xCD  # Символ конца посылки (в закодированном потоке)

MESS_CODER_ENC_START    = 0x1E  # Признак начала спец последовательности, после которого идет код спец последовательности (в закодированном потоке)
MESS_CODER_ENC_START_B  = 0x01  # Код совпадения с началом посылки (в закодированном потоке)
MESS_CODER_ENC_DATA_B   = 0x02  # Код совпадения с началом спец последовательности (в закодированном потоке)
MESS_CODER_ENC_END_B    = 0x03  # Код совпадения с концом посылки (в закодированном потоке)

MESS_CODER_RC_ERROR	    = -1    # Общая ошибка
MESS_CODER_RC_NO_START  = -21   # Символ начала посылки не найден
MESS_CODER_RC_NO_END    = -22   # Символ конца посылки не найден
MESS_CODER_RC_OVERFLOW  = -23   # Нехватка места в выходном буфере
MESS_CODER_RC_DECERR    = -24   # Ошибка декодирования ключевой последовательности


## \brief Кодирование данных
#
# \param[in,out] out Список закодированных данных
# \param[in] size_out Ограничение по размеру на закодированные данные
# \param[in] inp Список входных данных
# \param[in] size_inp Размер входных данных
# \return Размер закодированных данных; в случае ошибки - отрицательный код
def __encode(out: list, size_out: int, inp: list, size_inp: int) -> int:
    pass

## \brief Декодирование данных
#
# \param[in,out] out Список декодированных данных
# \param[in] size_out Ограничение по размеру на декодированные данные
# \param[in] inp Список входных данных
# \param[in] size_inp Размер входных данных
# \return Размер декодированных данных; в случае ошибки - отрицательный код
def __decode(out: list, size_out: int, inp: list, size_inp: int) -> int:
    pass

## \brief Функция, преобразующая блок данных
#  в поток для передачи по последовательному интерфейсу;
#  добавляет символы начала и конца посылки
#
# \param[in,out] out Список выходных данных
# \param[in] size_out Размер выходного потока данных
# \param[in] inp Список входных данных
# \param[in] size_inp Размер входного потока данных
# \return Положительный размер потока данных; в случае ошибки - отрицательный код
def to_serial(out: list, size_out: int, inp: list, size_inp: int) -> int:
    if (not inp) or (not out) or (not size_inp):
        return MESS_CODER_RC_ERROR
    return __encode(out, size_out, inp, size_inp)


## \brief Функция, преобразующая поток данных из последовательного интерфейса
#  в блок данных; убирает символы начала и конца посылки
#
# \param[in,out] out Список выходных данных
# \param[in] size_out Размер выходного потока данных
# \param[in] inp Список входных данных
# \param[in] size_inp Размер входного потока данных
# \return Положительный размер блока данных; в случае ошибки - отрицательный код
def from_serial(out: list, size_out: int, inp: list, size_inp: int) -> int:
    if (not inp) or (not out) or (not size_inp):
        return MESS_CODER_RC_ERROR
    return __decode(out, size_out, inp, size_inp)

## \brief Функция, рассчитывающая размер буфера,
#  который необходим для закодированного потока
#
# \param[in] inp Список входных данных
# \param[in] size_inp Размер входного потока данных
# \return Размер буфера
def comp_enc_size(inp: list, size_inp: int) -> int:
    pass


if __name__ == "__main__":
    print(f"{__file__}: It's not a program; it's a module")
