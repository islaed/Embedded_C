#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 15000
#define MAX_EVENTS 10


void get_current_time(char *buffer, size_t size)
{
    time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);

    char *msg = asctime(timeinfo);
    strncpy(buffer, msg, size - 1);
    buffer[size - 1] = '\0';
}


int main()
{
    int tcp_fd, udp_fd, epoll_fd;
    struct sockaddr_in server;
    struct epoll_event ev, events[MAX_EVENTS];

    // Создаём TCP сокет
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_fd == -1)
    {
        perror("TCP socket error!\n");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if(setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt TCP error!\n");
        close(tcp_fd);
        exit(EXIT_FAILURE);
    }

    // Задаём адрес и порт для TCP
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(SERVER_PORT);

    if(bind(tcp_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("TCP bind error!\n");
        close(tcp_fd);
        exit(EXIT_FAILURE);
    }

    if(listen(tcp_fd, 5) == -1)
    {
        perror("TCP listen error!\n");
        close(tcp_fd);
        exit(EXIT_FAILURE);
    }

    // Создаём UDP сокет
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_fd == -1)
    {
        perror("UDP socket error!\n");
        close(tcp_fd);
        exit(EXIT_FAILURE);
    }

    if(setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt UDP error!\n");
        close(tcp_fd);
        close(udp_fd);
        exit(EXIT_FAILURE);
    }

    // Используем тот же адрес и порт для UDP
    if(bind(udp_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("UDP bind error!\n");
        close(tcp_fd);
        close(udp_fd);
        exit(EXIT_FAILURE);
    }

    // Создаём epoll
    epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
    {
        perror("epoll_create1 error!\n");
        close(tcp_fd);
        close(udp_fd);
        exit(EXIT_FAILURE);
    }

    // Добавляем TCP сокет в epoll
    ev.events = EPOLLIN;
    ev.data.fd = tcp_fd;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_fd, &ev) == -1)
    {
        perror("epoll_ctl TCP error!\n");
        close(tcp_fd);
        close(udp_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    // Добавляем UDP сокет в epoll
    ev.events = EPOLLIN;
    ev.data.fd = udp_fd;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udp_fd, &ev) == -1)
    {
        perror("epoll_ctl UDP error!\n");
        close(tcp_fd);
        close(udp_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    printf("Мультипротокольный сервер запущен на %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Слушаем TCP и UDP соединения...\n");

    while(1)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if(nfds == -1)
        {
            perror("epoll_wait error!\n");
            break;
        }

        for(int i = 0; i < nfds; i++)
        {
            if(events[i].data.fd == tcp_fd)
            {
                // Новое TCP соединение
                struct sockaddr_in client;
                socklen_t client_len = sizeof(client);
                int client_fd = accept(tcp_fd, (struct sockaddr *)&client, &client_len);
                if(client_fd == -1)
                {
                    perror("accept error!\n");
                    continue;
                }

                printf("TCP клиент с портом %d подключился!\n",
                       ntohs(client.sin_port));

                // Добавляем клиентский сокет в epoll
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                {
                    perror("epoll_ctl client error!\n");
                    close(client_fd);
                    continue;
                }
            }
            else if(events[i].data.fd == udp_fd)
            {
                // UDP запрос
                struct sockaddr_in client;
                socklen_t client_len = sizeof(client);
                char buffer[256];

                ssize_t num_bytes = recvfrom(udp_fd, buffer, sizeof(buffer) - 1, 0,
                                              (struct sockaddr *)&client, &client_len);
                if(num_bytes == -1)
                {
                    perror("recvfrom error!\n");
                    continue;
                }

                buffer[num_bytes] = '\0';
                printf("UDP клиент с портом %d прислал запрос!\n",
                       ntohs(client.sin_port));

                // Получаем и отправляем время
                char time_buffer[256];
                get_current_time(time_buffer, sizeof(time_buffer));

                if(sendto(udp_fd, time_buffer, strlen(time_buffer), 0,
                          (struct sockaddr *)&client, client_len) == -1)
                {
                    perror("sendto error!\n");
                }
                else
                {
                    printf("UDP клиенту отправлено время.\n");
                }
            }
            else
            {
                // TCP клиент прислал данные
                int client_fd = events[i].data.fd;
                char buffer[256];

                ssize_t num_bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                if(num_bytes <= 0)
                {
                    if(num_bytes == 0)
                    {
                        printf("TCP клиент отключился.\n");
                    }
                    else
                    {
                        perror("recv error!\n");
                    }

                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    close(client_fd);
                    continue;
                }

                buffer[num_bytes] = '\0';
                printf("TCP клиент прислал запрос!\n");

                // Получаем и отправляем время
                char time_buffer[256];
                get_current_time(time_buffer, sizeof(time_buffer));

                if(send(client_fd, time_buffer, strlen(time_buffer), 0) == -1)
                {
                    perror("send error!\n");
                }
                else
                {
                    printf("TCP клиенту отправлено время.\n");
                }

                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                close(client_fd);
            }
        }
    }

    close(tcp_fd);
    close(udp_fd);
    close(epoll_fd);
    exit(EXIT_SUCCESS);
}
