#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "queue.h"


#define SERVER_IP "127.0.0.1"
#define LISTENING_SERVER_PORT 15000
#define POOL_SIZE 3


// Глобальная очередь и UDP socket
queue_t queue;
int udp_sock;


void *get_time(void *args)
{
    int thread_id = *(int *)args;

    printf("Поток %d включился.\n", thread_id + 1);

    while(1)
    {
        // Забираем адрес клиента из очереди (блокируемся если пусто)
        client_addr_t client;
        queue_get(&queue, &client);

        // Получаем текущее время
        time_t rawtime;
        time(&rawtime);

        struct tm *timeinfo;
        timeinfo = localtime(&rawtime);

        char *msg = asctime(timeinfo);

        // Отправляем время клиенту
        if(sendto(udp_sock, msg, strlen(msg), 0,
                  (struct sockaddr *)&client.addr, client.addr_len) == -1)
        {
            perror("sendto");
            continue;
        }

        printf("Поток %d отправил данные клиенту!\n", thread_id + 1);
    }

    return NULL;
}


int main()
{
    int server_fd;
    struct sockaddr_in server;
    char buffer[256];

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Задаём адрес и порт слушающему серверу
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(LISTENING_SERVER_PORT);

    if(bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Bind error!\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Инициализируем очередь
    if(queue_init(&queue) != 0)
    {
        fprintf(stderr, "Queue init error!\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    udp_sock = server_fd;

    pthread_t service_server[POOL_SIZE];
    int thread_id[POOL_SIZE];
    for(int i = 0; i < POOL_SIZE; i++)
    {
        thread_id[i] = i;
        pthread_create(&service_server[i], NULL, get_time, &thread_id[i]);
    }

    printf("Сервер включился и ожидает клиента.\n");

    while(1)
    {
        client_addr_t client;
        client.addr_len = sizeof(client.addr);

        // Получаем данные от клиента
        ssize_t num_bytes = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0,
                                      (struct sockaddr *)&client.addr,
                                      &client.addr_len);

        if(num_bytes == -1)
        {
            perror("recvfrom");
            continue;
        }

        buffer[num_bytes] = '\0';

        printf("Клиент с портом %d прислал запрос!\n",
               ntohs(client.addr.sin_port));

        // Добавляем адрес клиента в очередь
        queue_add(&queue, &client);
    }

    close(server_fd);
    queue_destroy(&queue);
    exit(EXIT_SUCCESS);
}