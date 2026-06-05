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
#define SERVER_PORT 15000


typedef struct {
    int server_fd;
    struct sockaddr_in client_addr;
    int thread_id;
} client_data_thread;


void *get_time(void *args)
{
    // Получаем структуру с данными клиента
    client_data_thread *client_data = (client_data_thread *) args;
    
    // Получаем текущее время
    time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);

    char *msg = asctime(timeinfo);
    if(sendto(client_data->server_fd, msg, strlen(msg), 0,
            (struct sockaddr *) &client_data->client_addr,
            sizeof(client_data->client_addr)) == -1)
    {
        perror("Thread send error!\n");
        free(client_data);
        exit(EXIT_FAILURE);
    }
    printf("Поток %d отправил данные клиенту!\n", client_data->thread_id);

    free(client_data);
    return NULL;
}


int main()
{
    int server_fd;
    int thread_id = 0;
    struct sockaddr_in server, client;
    socklen_t client_len;

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
    server.sin_port = htons(SERVER_PORT);

    if(bind(server_fd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        perror("Bind error!\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Сервер запущен!\n");

    while(1)
    {
        client_len = sizeof(client);

        // Принимаем заявку от клиента
        char buffer[256];
        int num_bytes = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0,
                        (struct sockaddr *) &client, &client_len);
        if(num_bytes == -1)
        {
            perror("Recv error!\n");
            continue;
        }
        buffer[num_bytes] = '\0';
        printf("От клиента с портом %d получен запрос!\n",
            ntohs(client.sin_port));
        thread_id += 1;
        
        // Выделяем память под структуру с данными
        client_data_thread *client_data = malloc(sizeof(client_data_thread));
        if(client_data == NULL)
        {
            perror("Malloc error!\n");
            continue;
        }

        // Заполняем структуру
        client_data->server_fd = server_fd;
        client_data->client_addr = client;
        client_data->thread_id = thread_id;

        // Создаём поток, который будет отправлять время клиенту
        pthread_t tid;
        pthread_create(&tid, NULL, get_time, client_data);
        printf("Создан поток №%d.\n", thread_id);
        pthread_detach(tid);
    }
    
    close(server_fd);
    exit(EXIT_SUCCESS);
}