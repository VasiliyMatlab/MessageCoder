#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_NAME   "chanell.fifo"

int main(void) {
    // Узнаем PID текущего процесса
    pid_t pid = getpid();
    // Код возврата текущего процесса
    int ret = 0;

    // Создаем именованный канал
    if (mkfifo(FIFO_NAME, 0777)) {
        perror("mkfifo failed");
        return errno;
    }
    fprintf(stdout, "[%d] %s is created\n", pid, FIFO_NAME);

    // Открываем канал на запись
    int fd = open(FIFO_NAME, O_WRONLY);
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
    for (int i = 0; i < 10; i++) {
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
