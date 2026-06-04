#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERV_PATH "./srv"
#define CLIENT_PATH "./clnt"

int main()
{
    int fd;
    struct sockaddr_un serv, clnt;
    socklen_t clnt_len = sizeof(clnt);

    // Назначаем файловый дескриптор
    fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Создаём серверный адрес
    memset(&serv, 0, sizeof(serv));
    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SERV_PATH, sizeof(serv.sun_path) - 1);
    
    unlink(SERV_PATH); // Удаляем файл, если он уже есть
    if(bind(fd, (struct sockaddr *) &serv,sizeof(serv)) == -1)
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

    // Завершаем работу с дескриптором и удаляем серверный файл
    close(fd);
    unlink(SERV_PATH);
    exit(EXIT_SUCCESS);
}