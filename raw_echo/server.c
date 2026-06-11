#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client_list.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 15000


// Создаём структуру udp заголовка, каждое поле по 2 байта
struct udpheader{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t check_sum;
};

// Счетчик клиентов
int client_counter = 1;

int main()
{
    int fd, get_port_fd; // get_port_fd нужен для того, чтобы занять порт в системе
    struct sockaddr_in client, get_port; 
    socklen_t client_len = sizeof(client);

    client_list_t client_list;
    client_list_init(&client_list);

    // Занимаем порт в системе
    get_port_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(get_port_fd == -1)
    {
        perror("Socket1 error!\n");
        exit(EXIT_FAILURE);
    }
    get_port.sin_family = AF_INET;
    get_port.sin_addr.s_addr = INADDR_ANY;
    get_port.sin_port = htons(SERVER_PORT);
    if(bind(get_port_fd, (struct sockaddr*)&get_port, sizeof(get_port)) == -1)
    {
        perror("Bind error!\n");
        exit(EXIT_FAILURE);
    }
    
    
    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    struct udpheader udp;
    // Заполняем часть udp заголовка, которая не зависит от клиента
    udp.source_port = htons(SERVER_PORT);
    udp.check_sum = 0;
    

    while(1)
    {
        char recv_buffer[2048];
        int num_bytes = recvfrom(fd, recv_buffer, sizeof(recv_buffer) - 1, 0, (struct sockaddr*)&client, &client_len);
        if(num_bytes == -1)
        {
            perror("Recv error!\n");
            exit(EXIT_FAILURE);
        }
        // Получаем udp заголовок, пропуская ip 20 байт
        struct udpheader *recv_udp = (struct udpheader*)(recv_buffer + 20);

        char *data = NULL;
        if(recv_udp->destination_port == htons(SERVER_PORT)) // Проверяем, что сообщение пришло на нужный порт
        {
            data = recv_buffer + 20 + 8; // Получаем payload, пропуская 20 байт ip + 8 байт заголовка udp
            data[num_bytes - 20 - 8] = '\0'; // Добавляем в конец сообщения '\0'
            // printf("Получено: %s\n", data);
        }
        else continue;

        if(strcmp(data, "EXIT") == 0)
        {
            int client_id = client_list_get_client_id(&client_list, client.sin_addr.s_addr, recv_udp->source_port);
            printf("Клиент %d отключился!\n", client_id);
            client_list_remove(&client_list, client.sin_addr.s_addr, recv_udp->source_port);
            continue;
        }

        if(client_list_exists(&client_list, client.sin_addr.s_addr, recv_udp->source_port))
        {
            // Если клиент уже есть
            int client_id = client_list_get_client_id(&client_list, client.sin_addr.s_addr, recv_udp->source_port);
            int msg_id = client_list_get_message_id(&client_list, client.sin_addr.s_addr, recv_udp->source_port);
            printf("Клиент %d посылает на сервер сообщение \"%s\"\n", client_id, data);

            char msg[strlen(data) + 20];
            sprintf(msg, "%s %d", data, msg_id);
            
            int total_len = sizeof(struct udpheader) + strlen(msg);
            udp.length = htons(total_len);
            udp.destination_port = recv_udp->source_port;

            // Заполняем буффер с udp заголовоком + наша строка
            char buffer[total_len];
            memcpy(buffer, &udp, sizeof(struct udpheader));
            memcpy(buffer + sizeof(struct udpheader), msg, strlen(msg));

            // Отправляем данные серверу
            if(sendto(fd, buffer, total_len, 0, (struct sockaddr*)&client, sizeof(client)) == -1)
            {
                perror("Send error!\n");
                exit(EXIT_FAILURE);
            }

            printf("Сервер отвечает клиенту %d \"%s\"\n", client_id, msg);
            client_list_increment_message_id(&client_list, client.sin_addr.s_addr, recv_udp->source_port);

        }
        else
        {
            // Если клиент появляется впервые
            client_list_add(&client_list, client.sin_addr.s_addr, recv_udp->source_port, client_counter);
            
            printf("Клиент %d посылает на сервер сообщение \"%s\"\n", client_counter, data);
            
            char msg[strlen(data) + 2];
            sprintf(msg, "%s %d", data, 1);
            
            int total_len = sizeof(struct udpheader) + strlen(msg);
            udp.length = htons(total_len);
            udp.destination_port = recv_udp->source_port;
            
            // Заполняем буффер с udp заголовоком + наша строка
            char buffer[total_len];
            memcpy(buffer, &udp, sizeof(struct udpheader));
            memcpy(buffer + sizeof(struct udpheader), msg, strlen(msg));
            
            // Отправляем данные серверу
            if(sendto(fd, buffer, total_len, 0, (struct sockaddr*)&client, sizeof(client)) == -1)
            {
                perror("Send error!\n");
                exit(EXIT_FAILURE);
            }
            
            printf("Сервер отвечает клиенту %d \"%s\"\n", client_counter, msg);
            client_list_increment_message_id(&client_list, client.sin_addr.s_addr, recv_udp->source_port);
            client_counter++;
        }

    }
    close(fd);
    close(get_port_fd);
    free(&client_list);
    exit(EXIT_SUCCESS);
}