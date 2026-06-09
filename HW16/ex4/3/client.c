#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 15000
#define CLIENT_PORT 16000


// Создаём структуру ip заголовка
struct ipheader{
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source_ip;
    uint32_t dest_ip;
};

// Создаём структуру udp заголовка, каждое поле по 2 байта
struct udpheader{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t check_sum;
};


int main()
{
    int fd;
    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(SERVER_PORT);

    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Включаем возможность заполнения ip заголовка вручную
    int enable = 1;
    if(setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable)) == -1)
    {
        perror("Setsockopt error!\n");
        exit(EXIT_FAILURE);
    }

    // Получаем строку от клиента
    printf("Введите сообщение: ");
    char msg[256];
    fgets(msg, sizeof(msg), stdin);
    msg[strlen(msg) - 1] = '\0'; // Заменяем \n на ничего

    int udp_len = sizeof(struct udpheader) + strlen(msg);
    int total_len = sizeof(struct ipheader) + udp_len;

    // Заполняем udp заголовок
    struct udpheader udp;
    udp.source_port = htons(CLIENT_PORT);
    udp.destination_port = htons(SERVER_PORT);
    udp.length = htons(udp_len);
    udp.check_sum = 0;

    // Заполняем ip заголовок
    struct ipheader ip;
    ip.version_ihl = 0x45; // version 4, ihl 5
    ip.tos = 0;
    ip.total_length = htons(total_len);
    ip.id = 0;
    ip.flags_offset = 0;
    ip.ttl = 64;
    ip.protocol = 17; // Порт udp протокола
    ip.checksum = 0;
    inet_pton(AF_INET, SERVER_IP, &ip.source_ip);
    inet_pton(AF_INET, SERVER_IP, &ip.dest_ip);

    // Заполняем буффер с ip заголовком + udp заголовоком + наша строка
    char buffer[total_len];
    memcpy(buffer, &ip, sizeof(struct ipheader));
    memcpy(buffer + sizeof(struct ipheader), &udp, sizeof(struct udpheader));
    memcpy(buffer + sizeof(struct ipheader) + sizeof(struct udpheader), msg, strlen(msg));

    // Отправляем данные серверу
    if(sendto(fd, buffer, total_len, 0, (struct sockaddr*)&server, sizeof(server)) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Строка отправлена!\n");

    // Принимаем данные
    while(1)
    {
        char recv_buffer[2048];
        int num_bytes = recvfrom(fd, recv_buffer, sizeof(recv_buffer) - 1, 0, (struct sockaddr*)&server, &server_len);
        if(num_bytes == -1)
        {
            perror("Recv error!\n");
            exit(EXIT_FAILURE);
        }
        // Получаем udp заголовок, пропуская ip 20 байт
        struct udpheader *recv_udp = (struct udpheader*)(recv_buffer + 20);

        // printf("Получено: %d\n", ntohs(recv_udp->destination_port));
        if(ntohs(recv_udp->destination_port) == CLIENT_PORT)
        {
            char *data = recv_buffer + 20 + 8; // Получаем payload, пропуская 20 байт ip + 8 байт заголовка udp
            data[num_bytes - 20 - 8] = '\0'; // Добавляем в конец сообщения '\0'
            printf("Получено: %s\n", data);
            exit(EXIT_SUCCESS);
        }
        else continue;
    }

    close(fd);
    exit(EXIT_SUCCESS);
}