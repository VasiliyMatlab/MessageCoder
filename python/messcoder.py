#!/usr/bin/env python3

"""messcoder.py: Message Coder module"""

__author__ = "VasiliyMatlab"
__copyright__ = "Vasiliy (c) 2023"


from enum import IntEnum

class Defines(IntEnum):
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
# \param[in] inp Список входных данных
# \return Список декодированных данных
def __encode(inp: list) -> list[int]:
    out = list()

    # Добавляем байт начала
    out.append(int(Defines.MESS_CODER_START_B))

    # Начинаем замену старт/стоп байтов
    for elem in inp:
        match elem:
            # Если встречаем символ начала посылки, то кодируем его
            case int(Defines.MESS_CODER_START_B):
                out.append(int(Defines.MESS_CODER_ENC_START))   # спец символ
                out.append(int(Defines.MESS_CODER_ENC_START_B)) # код начала посылки

            # Если встречаем спец символ, то кодируем его
            case int(Defines.MESS_CODER_ENC_START):
                out.append(int(Defines.MESS_CODER_ENC_START))   # спец символ
                out.append(int(Defines.MESS_CODER_ENC_DATA_B))  # код спец символа

            # Если встречаем символ конца посылки, то кодируем его
            case int(Defines.MESS_CODER_END_B):
                out.append(int(Defines.MESS_CODER_ENC_START))   # спец символ
                out.append(int(Defines.MESS_CODER_ENC_END_B))   # код конца посылки

            # Иначе оставляем символ как есть (без кодирования)
            case _:
                out.append(elem)

    # Добавляем байт окончания
    out.append(int(Defines.MESS_CODER_END_B))

    return out


## \brief Декодирование данных
#
# \param[in] inp Список входных данных
# \return Кортеж: код возврата и список декодированных данных
def __decode(inp: list) -> tuple[int, list[int]]:
    # Ищем байт начала потока
    idx = 0
    while idx < len(inp):
        if inp[idx] == int(Defines.MESS_CODER_START_B):
            break
        idx += 1
    else:
        return (int(Defines.MESS_CODER_RC_NO_START), list())
    
    # Начинаем поиск последовательностей кодов и замену на исходные байты
    out = list()
    try:
        while idx < (len(inp) - 1):
            match inp[idx]:
                # Нашли новое начало посылки;
                # выходной буфер и начинаем писать заново
                case int(Defines.MESS_CODER_START_B):
                    out.clear()

                # Нашли конец посылки
                case int(Defines.MESS_CODER_END_B):
                    raise StopIteration
                
                # Нашли байт начала кодовой последовательности;
                # расшифровываем следующий за ним байт для
                # восстановления исходной комбинации
                case int(Defines.MESS_CODER_ENC_START):
                    match inp[idx+1]:
                        # Следующий байт - код совпадения с началом посылки
                        case int(Defines.MESS_CODER_ENC_START_B):
                            out.append(int(Defines.MESS_CODER_START_B))
                            idx += 1
                        # Следующий байт - код совпадения со спец символом
                        case int(Defines.MESS_CODER_ENC_DATA_B):
                            out.append(int(Defines.MESS_CODER_ENC_START))
                            idx += 1
                        # Следующий байт - код совпадения с концом посылки
                        case int(Defines.MESS_CODER_ENC_END_B):
                            out.append(int(Defines.MESS_CODER_END_B))
                            idx += 1
                        # Неизвестная кодовая последовательность в
                        # закодированном потоке (если мы это получили,
                        # то это означает, что посылка была искажена
                        # или кодировщик отправки потока отработал
                        # неправильно)
                        case _:
                            print(f"Error: MESS_CODER: invalid code 0x{hex(inp[idx+1])}")
                            return (int(Defines.MESS_CODER_RC_DECERR), list())
                        
                # Нашли не закодированный байт
                case _:
                    out.append(inp[idx])

            idx += 1
    except StopIteration:
        pass

    # Проверяем последний символ; поток должен завершаться
	# символом конца, иначе - ошибка
    if inp[-1] != int(Defines.MESS_CODER_END_B):
        return (int(Defines.MESS_CODER_RC_NO_END), out)
    
    return (0, out)


## \brief Функция, преобразующая блок данных
#  в поток для передачи по последовательному интерфейсу;
#  добавляет символы начала и конца посылки
#
# \param[in] inp Список входных данных
# \return Кортеж: код возврата и список декодированных данных
def to_serial(inp: list) -> tuple[int, list[int]]:
    if not inp:
        return (int(Defines.MESS_CODER_RC_ERROR), list())
    return (0, __encode(inp))


## \brief Функция, преобразующая поток данных из последовательного интерфейса
#  в блок данных; убирает символы начала и конца посылки
#
# \param[in] inp Список входных данных
# \return Кортеж: код возврата и список декодированных данных
def from_serial(inp: list) -> tuple[int, list[int]]:
    if not inp:
        return (int(Defines.MESS_CODER_RC_ERROR), list())
    return __decode(inp)


## \brief Функция, рассчитывающая размер буфера,
#  который необходим для закодированного потока
#
# \param[in] inp Список входных данных
# \return Размер буфера
def comp_enc_size(inp: list[int]) -> int:
    ostream_size = 2
    for elem in inp:
        match elem:
            case int(Defines.MESS_CODER_START_B) | \
                 int(Defines.MESS_CODER_END_B) | \
                 int(Defines.MESS_CODER_ENC_START):
                ostream_size += 2
            case _:
                ostream_size += 1
    return ostream_size


if __name__ == "__main__":
    print(f"{__file__}: It's not a program; it's a module")
