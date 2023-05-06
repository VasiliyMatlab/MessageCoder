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
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_NAME   "chanell.fifo"  ///< Название именнованного канала

// PID текущего процесса
pid_t pid;
// Дескриптор именованного канала
int fd;

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

int main(void) {
    // Узнаем PID
    pid = getpid();
    // Код возврата текущего процесса
    int ret = 0;

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

    // Пишем в канал данные    
    char buf[] = "test_string";
    for (int i = 0; i < 10; i++) {
        ssize_t bytes = write(fd, buf, sizeof(buf));
        if (bytes == -1) {
            perror("write failed");
            ret = errno;
            break;
        }
        fprintf(stdout, "[%d] Data is written to %s (%ld bytes): %s\n", pid, FIFO_NAME, bytes, buf);
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
