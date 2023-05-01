/*
 * file:        main.c
 * author:      VasiliyMatlab
 * version:     1.0
 * date:        01.05.2023
 * copyright:   Vasiliy (c) 2023
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_NAME   "chanell.fifo"  ///< Название именнованного канала

// PID текущего процесса
pid_t pid;
// Дескриптор именованного канала
int fd;
// Код возврата текущего процесса
int ret = 0;
// Признак работы программы
int running = 1;

void signal_handler(int __attribute__((unused)) signalno) {
    running = 0;
}

int main(void) {
    // Узнаем PID
    pid = getpid();

    // Задаем обработчик сигнала
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
    char buf[] = "test_string\n";
    for (int i = 0; (i < 10) && running; i++) {
        ssize_t bytes = write(fd, buf, sizeof(buf));
        if (bytes == -1) {
            perror("write failed");
            ret = errno;
            break;
        }
        fprintf(stdout, "[%d] Data is written to %s (%ld bytes): %s", pid, FIFO_NAME, bytes, buf);
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
