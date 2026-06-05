#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{
    int fd;
    struct sockaddr_in serv;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Задаём данные сервера, к которому хотим подключиться
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);
    serv.sin_port = htons(8080);

    // Подключаемся к серверу
    if(connect(fd, (struct sockaddr *) &serv, sizeof(serv)) == -1)
    {
        perror("Connect error!\n");
        close(fd);
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

    // Принимаем сообщение от сервера
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