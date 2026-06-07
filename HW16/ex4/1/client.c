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

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(SERVER_PORT);

    if(connect(fd, (struct sockaddr*)&server, sizeof(server)) == -1)
    {
        perror("Connect error!\n");
        exit(EXIT_FAILURE);
    }

    // Отправляем строку серверу
    printf("Введите сообщение: ");
    char msg[256];
    fgets(msg, sizeof(msg), stdin);
    msg[strlen(msg) - 1] = '\0'; // Заменяем \n на ничего
    if(send(fd, msg, strlen(msg), 0) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено серверу: %s\n", msg);

    // Принимаем модифицированную строку от сервера
    char buf[256];
    int num_bytes = recv(fd, buf,sizeof(buf) - 1, 0);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE);
    }
    buf[num_bytes] = '\0';
    printf("Получено: %s\n", buf);

    close(fd);
    exit(EXIT_SUCCESS);
}