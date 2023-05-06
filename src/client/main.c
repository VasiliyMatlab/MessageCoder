/*
 * file:        main.c
 * author:      VasiliyMatlab
 * version:     1.1
 * date:        06.05.2023
 * copyright:   Vasiliy (c) 2023
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FIFO_NAME   "chanell.fifo"  ///< Название именнованного канала

// PID текущего процесса
pid_t pid;
// Дескриптор именованного канала
int fd;

void signal_handler(int __attribute__((unused)) signalno) {
    // Закрываем канал
    if (close(fd)) {
        perror("close failed");
        exit(errno);
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, FIFO_NAME);
    exit(EXIT_SUCCESS);
}

int main(void) {
    // Узнаем PID текущего процесса
    pid = getpid();
    // Код возврата текущего процесса
    int ret = 0;

    // Задаем обработчик сигналов
    signal(SIGINT, signal_handler);

    // Открываем канал на чтение
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is opened\n", pid, FIFO_NAME);

    // Читаем данные из канала
    char buf[BUFSIZ] = {0};
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
        fprintf(stdout, "[%d] Data is read from %s (%ld bytes): %s\n", pid, FIFO_NAME, bytes, buf);
    }

    // Закрываем канал
    if (close(fd)) {
        perror("close failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is closed\n", pid, FIFO_NAME);

    return ret;
}
