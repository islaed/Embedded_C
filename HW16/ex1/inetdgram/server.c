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
    struct sockaddr_in serv, clnt;
    socklen_t clnt_len = sizeof(clnt);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Задаём адрес и порт серверу
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);
    serv.sin_port = htons(8080);

    if(bind(fd, (struct sockaddr *) &serv, sizeof(serv)) == -1)
    {
        perror("Bind error!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Принимаем сообщение от клиента
    char buf[256];
    int num_bytes = recvfrom(fd, buf,sizeof(buf) - 1, 0,
            (struct sockaddr *) &clnt, &clnt_len);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE);
    }
    buf[num_bytes] = '\0';
    printf("Получено: %s\n", buf);

    // Отправляем сообщение клиенту
    char *msg = "Hi!";
    if(sendto(fd, msg, strlen(msg), 0,
            (struct sockaddr *) &clnt, clnt_len) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено: %s\n", msg);

    close(fd);
    exit(EXIT_SUCCESS);
}