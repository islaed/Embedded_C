#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERV_PATH "./srv"

int main()
{
    int fd;
    struct sockaddr_un serv;

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Подключаемся к серверу
    memset(&serv, 0, sizeof(serv));
    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SERV_PATH, sizeof(serv.sun_path) - 1);

    if(connect(fd, (struct sockaddr*) &serv, sizeof(serv)) == -1)
    {
        perror("Connect error!\n");
        exit(EXIT_FAILURE);
    }

    // Отправляем сообщение серверу
    char *msg = "Hello!";
    if(send(fd, msg, strlen(msg), 0) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено: %s\n", msg);

    // Получаем сообщение от сервера
    char buf[256];
    int num_bytes = recv(fd, buf, sizeof(buf), 0);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE);
    }
    buf[num_bytes] = '\0';
    printf("Получено: %s\n", buf);

    // Завершаем работу 
    close(fd);
    exit(EXIT_SUCCESS);
}