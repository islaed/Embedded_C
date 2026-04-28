#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
    char *fifo = "./named_pipe";
    char *message = "Hi!";

    mkfifo(fifo, 0666); // Создаем именованный канал с правами доступа 0666

    printf("Сервер блокируется и ожидает клиента\n");
    int fd = open(fifo, O_WRONLY); // Открываем канал только на запись
    
    write(fd, message, 4); // Записываем строку "Hi!" (включая \0)
    close(fd);
    
    return 0;
}