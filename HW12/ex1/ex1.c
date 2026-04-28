#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    pid_t proc;
    char message_buffer[128];
    char *message = "Hi!";

    pipe(fd); // Создаем неименованный канал
    proc = fork(); // Порождаем процесс

    if(proc > 0)
    {
        close(fd[0]); // Закрываем чтение

        write(fd[1], message, strlen(message) + 1); // Записываем в канал строку
        close(fd[1]); // Закончили запись

        wait(NULL);   // Ждем завершения дочернего процесса
    } 
    else
    {
        close(fd[1]); // Закрываем запись

        read(fd[0], message_buffer, sizeof(message_buffer)); // Читаем строку из канала
        printf("Дочерний процесс прочитал: %s\n", message_buffer);
        close(fd[0]); // Закончили чтение

        exit(0);
    }

    return 0;
}