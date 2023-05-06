/*
 * file:        main.c
 * author:      VasiliyMatlab
 * version:     1.2
 * date:        06.05.2023
 * copyright:   Vasiliy (c) 2023
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define MIN_ROWS    4               ///< Минимальное количество строк данных
#define MAX_ROWS    16              ///< Максимальное количество строк данных
#define MIN_COLS    8               ///< Минимальное количество столбцов данных
#define MAX_COLS    64              ///< Максимальное количество столбцов данных
#define FIFO_NAME   "chanell.fifo"  ///< Название именнованного канала

// PID текущего процесса
pid_t pid;
// Дескриптор именованного канала
int fd;

/**
 * \brief Обработчик сигналов
 * 
 * \param[in] signalno Поступивший сигнал
 */
void signal_handler(int __attribute__((unused)) signalno) {
    // Закрываем канал
    if (close(fd)) {
        perror("close failed");
        int ret = errno;
        if (remove(FIFO_NAME)) {
            perror("remove failed");
        }
        exit(ret);
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, FIFO_NAME);

    // Удаляем именнованный канал
    if (remove(FIFO_NAME)) {
        perror("remove failed");
        exit(errno);
    }
    fprintf(stdout, "[%d] %s is removed\n", pid, FIFO_NAME);
    exit(EXIT_SUCCESS);
}

/**
 * \brief Функция генерации данных
 * 
 * \param[in,out] buf Буфер, куда будут помещаться данные
 * \param[in,out] cols Количество столбцов в строках
 * \return Количество строк
 */
uint8_t generate_data(uint8_t buf[MAX_ROWS][MAX_COLS], uint8_t cols[MAX_ROWS]) {
    uint8_t rows = (rand() % (MAX_ROWS - MIN_ROWS)) + MIN_ROWS;
    for (uint8_t i = 0; i < rows; i++) {
        cols[i] = (rand() % (MAX_COLS - MIN_COLS)) + MIN_COLS;
        for (uint8_t j = 0; j< cols[i]; j++) {
            buf[i][j] = (uint8_t) rand();
        }
    }
    return rows;
}

/**
 * \brief Функция main
 * 
 * \return Код возврата 
 */
int main(void) {
    // Узнаем PID
    pid = getpid();
    // Код возврата текущего процесса
    int ret = 0;
    // Инициализируем генератор случайных чисел
    srand(time(NULL));

    // Задаем обработчик сигналов
    signal(SIGPIPE, signal_handler);

    // Создаем именованный канал
    if (mkfifo(FIFO_NAME, 0777)) {
        perror("mkfifo failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is created\n", pid, FIFO_NAME);

    // Открываем канал на запись
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd < 0) {
        perror("open failed");
        ret = errno;
        if (remove(FIFO_NAME)) {
            perror("remove failed");
        }
        return ret;
    }
    fprintf(stdout, "[%d] %s is opened\n", pid, FIFO_NAME);

    // Генерация данных
    uint8_t rows = 0;
    uint8_t dec_cols[MAX_ROWS] = {0};
    uint8_t dec_data[MAX_ROWS][MAX_COLS] = {0};
    rows = generate_data(dec_data, dec_cols);

    // Пишем в канал данные
    for (uint8_t i = 0; i < rows; i++) {
        ssize_t bytes = write(fd, dec_data[i], dec_cols[i]);
        if (bytes == -1) {
            perror("write failed");
            ret = errno;
            break;
        }
        fprintf(stdout, "[%d] Data is written to %s (%ld bytes): 0x", pid, FIFO_NAME, bytes);
        for (uint8_t j = 0; j < bytes; j++) {
            fprintf(stdout, "%02hhX ", dec_data[i][j]);
        }
        fprintf(stdout, "\n");
        sleep(1);
    }

    // Закрываем канал
    if (close(fd)) {
        perror("close failed");
        ret = errno;
        if (remove(FIFO_NAME)) {
            perror("remove failed");
        }
        return ret;
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, FIFO_NAME);

    // Удаляем именнованный канал
    if (remove(FIFO_NAME)) {
        perror("remove failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is removed\n", pid, FIFO_NAME);

    return ret;
}
