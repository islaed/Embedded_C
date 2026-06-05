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

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Задаём адрес и порт серверу
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(SERVER_PORT);

    // Отправляем запрос серверу
    char msg[] = "Hello!";
    if(sendto(fd, msg, strlen(msg), 0,
              (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Send error!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Запрос отправлен серверу.\n");

    // Получаем текущее время
    char buf[256];
    socklen_t server_len = sizeof(server);
    int num_bytes = recvfrom(fd, buf, sizeof(buf) - 1, 0,
                              (struct sockaddr *)&server, &server_len);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    buf[num_bytes] = '\0';
    printf("Текущее время: %s\n", buf);

    close(fd);
    exit(EXIT_SUCCESS);
}
