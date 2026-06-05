#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_IP "127.0.0.1"
#define LISTENING_SERVER_PORT 15000
#define POOL_SIZE 3

int server_is_busy[POOL_SIZE];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *get_time(void *args)
{
    int thread_id = *(int *)args;

    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        perror("Thread socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Задаём адрес и порт обслуживающему серверу
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(LISTENING_SERVER_PORT + thread_id + 1);
    
    if(bind(server_fd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        perror("Thread bind error!\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    listen(server_fd, POOL_SIZE);

    printf("Поток %d включился.\n", thread_id + 1);

    while(1)
    {
        client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
        if(client_fd == -1)
        {
            perror("Thread accept error!\n");
            continue;
        }
        printf("Клиент подключился.\n");

        // Получаем текущее время
        time_t rawtime;
        time(&rawtime);

        struct tm *timeinfo;
        timeinfo = localtime(&rawtime);

        char *msg = asctime(timeinfo);
        if(send(client_fd, msg, strlen(msg), 0) == -1)
        {
            perror("Send error!\n");
            continue;
        }
        close(client_fd);
        pthread_mutex_lock(&lock);
        server_is_busy[thread_id] = 0;
        pthread_mutex_unlock(&lock);
        printf("Поток %d отправил данные клиенту!\n", thread_id + 1);
    }
}


int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    if(bind(server_fd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        perror("Bind error!\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    listen(server_fd, POOL_SIZE);

    pthread_t service_server[POOL_SIZE];
    int thread_id[POOL_SIZE];
    for(int i = 0; i < POOL_SIZE; i++)
    {
        server_is_busy[i] = 0;
        thread_id[i] = i;
        pthread_create(&service_server[i], NULL, get_time, &thread_id[i]);
    }

    printf("Сервер включился и ожидает клиента.\n");

    while(1)
    {
        printf("Занятость потоков:\n");
        for(int i = 0; i < POOL_SIZE; i++)
        {
            printf("Поток %d: %d\n", i + 1, server_is_busy[i]);
        }
        client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
        if(client_fd == -1)
        {
            perror("Accept error!\n");
            continue;
        }

        pthread_mutex_lock(&lock);
        int found = 0;
        for(int i = 0; i < POOL_SIZE; i++)
        {
            if(server_is_busy[i]) continue; // Если занят (1), то пропускаем
            else
            {
                // Отправляем клиенту порт свободного потока
                int port = LISTENING_SERVER_PORT + i + 1;
                if(send(client_fd, &port, sizeof(port), 0) == -1)
                {
                    perror("Send error!\n");
                    break;
                }
                // Если всё успешно отправилось, то меняем состояние потока на занятое
                server_is_busy[i] = 1;
                printf("Клиент с портом %d направлен к потоку %d!\n", 
                        ntohs(client.sin_port),
                        i + 1);
                found = 1;
                break;
            }
        }
        if(!found)
        {
            printf("Все потоки заняты!\n");
            int port = 0;
            send(client_fd, &port, sizeof(port), 0);
        }
        close(client_fd);
        pthread_mutex_unlock(&lock);
    }

    close(server_fd);
    close(client_fd);
    exit(EXIT_SUCCESS);
}