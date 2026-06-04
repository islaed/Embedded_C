#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERV_PATH "./srv"

int main()
{
    int fd, clnt_fd;
    struct sockaddr_un serv, clnt;
    socklen_t clnt_len = sizeof(clnt);

    // Назначаем файловый дескриптор
    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Создаём серверный адрес
    memset(&serv, 0, sizeof(serv));
    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SERV_PATH, sizeof(serv.sun_path) - 1);

    unlink(SERV_PATH);
    if(bind(fd, (struct sockaddr *) &serv, sizeof(serv)) == -1)
    {
        perror("Bind error!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    listen(fd, 5); // Ожидаем клиентов

    // Получение дескриптора клиента
    clnt_fd = accept(fd, (struct sockaddr *) &clnt, &clnt_len);
    if(clnt_fd == -1)
    {
        perror("Accept error!\n");
        exit(EXIT_FAILURE);
    }

    // Получаем сообщение от клиента
    char buf[256];
    int num_bytes = recv(clnt_fd, buf, sizeof(buf), 0);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE);
    }
    buf[num_bytes] = '\0';
    printf("Получено: %s\n", buf);

    // Отправка сообщения клиенту
    char *msg = "Hi!";
    if(send(clnt_fd, msg, strlen(msg), 0) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено: %s\n", msg);

    // Завершаем работу
    close(fd);
    close(clnt_fd);
    unlink(SERV_PATH);
    exit(EXIT_SUCCESS);
}