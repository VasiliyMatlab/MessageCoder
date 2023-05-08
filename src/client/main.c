/*
 * file:        main.c
 * author:      VasiliyMatlab
 * version:     1.4
 * date:        08.05.2023
 * copyright:   Vasiliy (c) 2023
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mess_coder.h>

#include "rbuf.h"

#define MIN_MSG     8       ///< Минимальная длина принимаемого сообщения
#define MAX_MSG     64      ///< Максимальная длина принимаемого сообщения

#define MIN_ENC_MSG (1 + MIN_MSG + 1)   ///< Минимальная длина закодированного сообщения
#define MAX_ENC_NSG (1 + 2*MAX_MSG + 1) ///< Максимальная длина закодированного сообщения

#define FIFO_NAME   "chanell.fifo"      ///< Название именнованного канала

/// PID текущего процесса
pid_t pid;
/// Дескриптор именованного канала
int fd;
/// Название именованного канала
char fifo_name[32] = FIFO_NAME;

/**
 * \brief Обработчик сигналов
 * 
 * \param[in] signalno Поступивший сигнал
 */
void signal_handler(int __attribute__((unused)) signalno) {
    // Закрываем канал
    if (close(fd)) {
        perror("close failed");
        exit(errno);
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, fifo_name);
    exit(EXIT_SUCCESS);
}

/**
 * \brief Функция вывода справки в стандартный поток вывода
 * 
 * \param[in] argv0 Название исполняемого файла
 */
void print_usage(const char *argv0) {
    fprintf(stdout, "Usage: %s [OPTION]\n", argv0);
    fprintf(stdout, "-h             print this help\n");
    fprintf(stdout, "-f <fifoname>  set fifo filename\n");
    exit(EXIT_SUCCESS);
}

/**
 * \brief Функция main
 * 
 * \param[in] argc Количество принятых аргументов
 * \param[in] argv Аргументы командной строки
 * \return Код возврата
 */
int main(int argc, char *argv[]) {
    // Парсим аргументы командной строки
    int opt;
    while ((opt = getopt(argc, argv, "hf:")) != -1) {
        switch (opt) {
        case 'h':
            print_usage(argv[0]);
            break;
        case 'f':
            strcpy(fifo_name, optarg);
            break;
        default:
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Узнаем PID текущего процесса
    pid = getpid();
    // Код возврата текущего процесса
    int ret = 0;

    // Задаем обработчик сигналов
    signal(SIGINT, signal_handler);

    // Инициализируем кольцевой буфер
    struct rbuf rcv_rbuf;
    if (rbuf_init(&rcv_rbuf)) {
        fprintf(stderr, "rbuf_init failed\n");
        return -1;
    }

    // Открываем канал на чтение
    fd = open(fifo_name, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is opened\n", pid, fifo_name);

    // Читаем данные из канала
    uint8_t buf[BUFSIZ] = {0};
    uint8_t pkgs = 0, msgs = 0;
    while (1) {
        memset(buf, 0, BUFSIZ);
        ssize_t bytes = read(fd, buf, BUFSIZ - 1);

        if (bytes == -1) {
            perror("read failed");
            ret = errno;
            break;
        }

        if (bytes == 0) {
            fprintf(stdout, "[%d] The end of transmit is reached\n", pid);
            break;
        }
        pkgs++;

        // Пишем в кольцевой буфер принятые байты
        bytes = rbuf_write(&rcv_rbuf, buf, bytes);
        if (bytes == 0) {
            continue;
        }

        // Обрабатываем все, что приняли в кольцевой буфер
        while (1) {
            // Ищем начало сообщения
            bytes = rbuf_search(&rcv_rbuf, MESS_CODER_START_B);
            // Если начало не найдено
			if (bytes < 0) {
				rbuf_drop(&rcv_rbuf);
				break;
			}
			// Сдвиг на начало
			rbuf_shift(&rcv_rbuf, bytes);
			// Ищем конец посылки
			bytes = rbuf_search(&rcv_rbuf, MESS_CODER_END_B);
			// Если конец не найден
			if (bytes < 0) {
				// Проверяем текущий размер буфера
				if (rbuf_get_size_used(&rcv_rbuf) > MAX_ENC_NSG) {
					rbuf_drop(&rcv_rbuf);
				}
				break;
			}

            // Пересчитываем индекс в длину
			size_t enc_len = bytes + 1;
			// Если недопустимая длина прочитанного сообщения
			if ((enc_len < MIN_ENC_MSG) || (enc_len > MAX_ENC_NSG)) {
				fprintf(stderr, "Error: invalid received message size %lu\r\n", enc_len);
				// Было и начало, и конец, но размер не правильный
				// Возможно, сообщение с потерянным концом:
				// 1. Отбрасываем текущий стартовый символ
				// 2. Ищем следующий стартовый символ
				rbuf_shift(&rcv_rbuf, 1);
				
				bytes = rbuf_search(&rcv_rbuf, MESS_CODER_START_B);
				if (bytes >= 0) {
					rbuf_shift(&rcv_rbuf, bytes);
				} else {
					rbuf_shift(&rcv_rbuf, enc_len);
				}

				continue;
			}

            // Читаем закодированное сообщение из кольцевого буфера
            uint8_t enc_msg[MAX_ENC_NSG] = {0};
            enc_len = rbuf_read(&rcv_rbuf, enc_msg, enc_len);
            rbuf_shift(&rcv_rbuf, enc_len);

            // Декодирование сообщения
            uint8_t dec_msg[MAX_MSG] = {0};
            ssize_t dec_len = messcoder_from_serial(dec_msg, MAX_MSG, enc_msg, enc_len);
            if (dec_len < 1) {
                fprintf(stderr, "messcoder_from_serial failed with code %ld\n", dec_len);
                continue;
            }
            msgs++;

            // Печатаем сообщение в стандартный поток вывода
            fprintf(stdout, "[%d] Message is read from %s (%ld bytes): 0x", pid, fifo_name, dec_len);
            for (uint8_t i = 0; i < dec_len; i++) {
                fprintf(stdout, "%02hhX ", dec_msg[i]);
            }
            fprintf(stdout, "\n");
        }
    }

    fprintf(stdout, "[%d] Total packages %u (messages %u)\n", pid, pkgs, msgs);

    // Закрываем канал
    if (close(fd)) {
        perror("close failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, fifo_name);

    return ret;
}
