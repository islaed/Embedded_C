#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


int main()
{
    char *fifo_path = "./named_pipe";
    char message_buffer[128];

    // Открываем канал только на чтение (O_RDONLY)
    int fd = open(fifo_path, O_RDONLY);
    if (fd == -1)
    {
        printf("Сервер не запущен!\n");
        return 1;
    }

    // Читаем данные из канала
    read(fd, message_buffer, sizeof(message_buffer));
    printf("Сообщение: %s\n", message_buffer);
    close(fd);

    unlink(fifo_path); // Удаляем именованный канал
    printf("Именованый канал удалён.\n");

    return 0;
}