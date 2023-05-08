/*
 * file:        main.c
 * author:      VasiliyMatlab
 * version:     1.5
 * date:        06.05.2023
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
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include <mess_coder.h>

#define MIN_ROWS    4       ///< Минимальное количество строк данных
#define MAX_ROWS    16      ///< Максимальное количество строк данных
#define MIN_COLS    8       ///< Минимальное количество столбцов данных
#define MAX_COLS    64      ///< Максимальное количество столбцов данных
#define MIN_MSG     4       ///< Минимальная длина отправляемого сообщения
#define MAX_MSG     64      ///< Максимальная длина отправляемого сообщения

#define MAX_ENC_COLS    (1 + 2 * MAX_COLS + 1)                      ///< Максимальное количество столбцов закодированных данных
#define MAX_SPLIT_ROWS  ((MAX_ROWS * MAX_ENC_COLS) / MIN_MSG + 1)   ///< Максимальное количество строк разбитых данных

#define FIFO_NAME   "chanell.fifo"  ///< Название именнованного канала по умолчанию

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
        int ret = errno;
        if (remove(fifo_name)) {
            perror("remove failed");
        }
        exit(ret);
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, fifo_name);

    // Удаляем именнованный канал
    if (remove(fifo_name)) {
        perror("remove failed");
        exit(errno);
    }
    fprintf(stdout, "[%d] %s is removed\n", pid, fifo_name);
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
 * \brief Функция кодирования данных
 * 
 * \param[in] rows Количество строк с данными
 * \param[in] buf_in Буфер, откуда берутся данные
 * \param[in] cols_in Количество столбцов в строках исходных данных
 * \param[in,out] buf_out Буфер с закодированными данными
 * \param[in,out] cols_out Количество столбцов в строках закодированных данных
 * \return 0 в случае успешного выполнения; иначе код ошибки
 */
int encode_data(const uint8_t rows,
                const uint8_t buf_in[MAX_ROWS][MAX_COLS],
                const uint8_t cols_in[MAX_ROWS],
                uint8_t buf_out[MAX_ROWS][MAX_ENC_COLS],
                uint8_t cols_out[MAX_ROWS]) {
    for (uint8_t i = 0; i < rows; i++) {
        int size = messcoder_comp_enc_size(buf_in, MAX_COLS);
        size = messcoder_to_serial(buf_out[i], size, buf_in[i], cols_in[i]);
        if (size < 0)
            return size;
        cols_out[i] = size;
    }
    return 0;
}

/**
 * \brief Функция разбиения данных
 * 
 * \param[in] rows Количество строк с данными
 * \param[in] buf_in Буфер, откуда берутся данные
 * \param[in] cols_in Количество столбцов в строках исходных данных
 * \param[in,out] buf_out Буфер с разбитыми данными
 * \param[in,out] cols_out Количество столбцов в строках разбитых данных
 * \return Количество строк с разбитыми данными
 */
uint16_t split_data(const uint8_t rows,
                    const uint8_t buf_in[MAX_ROWS][MAX_ENC_COLS],
                    const uint8_t cols_in[MAX_ROWS],
                    uint8_t buf_out[MAX_SPLIT_ROWS][MAX_MSG],
                    uint8_t cols_out[MAX_SPLIT_ROWS]) {
    int16_t total_bytes = 0;
    for (uint8_t i = 0; i < rows; i++) {
        total_bytes += cols_in[i];
    }
    uint8_t curr_idx = 0;
    uint8_t curr_row = 0, curr_col = 0;
    while (total_bytes > 0) {
        uint8_t curr_msg_size = (rand() % (MAX_MSG - MIN_MSG)) + MIN_MSG;
        curr_msg_size = (curr_msg_size > total_bytes) ? total_bytes : curr_msg_size;
        for (uint8_t i = 0; i < curr_msg_size; i++, total_bytes--) {
            buf_out[curr_idx][i] = buf_in[curr_row][curr_col++];
            if (curr_col == cols_in[curr_row]) {
                curr_col = 0;
                curr_row++;
            }
        }
        cols_out[curr_idx] = curr_msg_size;
        curr_idx++;
    }
    return curr_idx;
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

    // Узнаем PID
    pid = getpid();
    // Код возврата текущего процесса
    int ret = 0;
    // Инициализируем генератор случайных чисел
    srand(time(NULL));

    // Задаем обработчик сигналов
    signal(SIGPIPE, signal_handler);
    signal(SIGINT,  signal_handler);

    // Создаем именованный канал
    if (mkfifo(fifo_name, 0777)) {
        perror("mkfifo failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is created\n", pid, fifo_name);

    // Открываем канал на запись
    fd = open(fifo_name, O_WRONLY);
    if (fd < 0) {
        perror("open failed");
        ret = errno;
        if (remove(fifo_name)) {
            perror("remove failed");
        }
        return ret;
    }
    fprintf(stdout, "[%d] %s is opened\n", pid, fifo_name);

    // Генерация данных
    uint8_t rows = 0;
    uint8_t dec_cols[MAX_ROWS] = {0};
    uint8_t dec_data[MAX_ROWS][MAX_COLS] = {0};
    rows = generate_data(dec_data, dec_cols);

    // Кодирование данных
    uint8_t enc_cols[MAX_ROWS] = {0};
    uint8_t enc_data[MAX_ROWS][MAX_ENC_COLS] = {0};
    ret = encode_data(rows, dec_data, dec_cols, enc_data, enc_cols);
    if (ret) {
        fprintf(stderr, "encode failed with code %d\n", ret);
        goto end_work;
    }

    // Разбиение данных
    uint8_t spl_cols[MAX_SPLIT_ROWS] = {0};
    uint8_t spl_data[MAX_SPLIT_ROWS][MAX_MSG] = {0};
    uint16_t spl_rows = split_data(rows, enc_data, enc_cols, spl_data, spl_cols);

    // Пишем в канал данные
    fprintf(stdout, "[%d] Total packages %u (messages %u)\n", pid, spl_rows, rows);
    for (uint8_t i = 0; i < spl_rows; i++) {
        ssize_t bytes = write(fd, spl_data[i], spl_cols[i]);
        if (bytes == -1) {
            perror("write failed");
            ret = errno;
            break;
        }
        fprintf(stdout, "[%d] Data is written to %s (%ld bytes): 0x", pid, fifo_name, bytes);
        for (uint8_t j = 0; j < bytes; j++) {
            fprintf(stdout, "%02hhX ", spl_data[i][j]);
        }
        fprintf(stdout, "\n");
        sleep(1);
    }

end_work:
    // Закрываем канал
    if (close(fd)) {
        perror("close failed");
        ret = errno;
        if (remove(fifo_name)) {
            perror("remove failed");
        }
        return ret;
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, fifo_name);

    // Удаляем именнованный канал
    if (remove(fifo_name)) {
        perror("remove failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is removed\n", pid, fifo_name);

    return ret;
}
