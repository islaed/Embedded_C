#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 15000


int main()
{
    int fd;
    struct sockaddr_in server;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Задаём адрес и порт слушающему серверу
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(SERVER_PORT);

    if(connect(fd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        perror("Bind error!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Принимаем сообщение c портом от сервера
    int port;
    int num_bytes = recv(fd, &port, sizeof(port), 0);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE);
    }
    if(port == 0)
    {
        printf("Сервер занят!\n");
        exit(EXIT_FAILURE);
    }

    close(fd);
    printf("Подключаюсь к серверу %d.\n", port);

    // Подключаемся к обслуживающему серверу
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Задаём адрес и порт слушающему серверу
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(port);

    if(connect(fd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        perror("Bind error!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Получаем текущее время
    char buf[256];
    num_bytes = recv(fd, buf,sizeof(buf) - 1, 0);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE);
    }
    buf[num_bytes] = '\0';
    printf("Текущее время: %s\n", buf);

    close(fd);
    exit(EXIT_SUCCESS);
}