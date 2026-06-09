#define _GNU_SOURCE // Без него struct ifreq выделяется как ошибка
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>


#define SERVER_IP "192.168.0.41" // Это адрес моего другого компьютера
#define SERVER_PORT 15000
#define CLIENT_PORT 16000
#define INTERFACE "wlp0s20f3" // Имя моего интерфейса


// Создаём структуру ethernet заголовка
struct ethheader{
    uint8_t dest_mac[6];
    uint8_t source_mac[6];
    uint16_t ethertype;
};

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


// Нашел в интернете функцию по подсчету контрольной суммы
uint16_t calculate_checksum(uint16_t *buf, int len)
{
    uint32_t sum = 0;

    for(int i = 0; i < len / 2; i++)
        sum += buf[i];

    if(len % 2)
        sum += *((uint8_t*)buf + len - 1);

    while(sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ~sum;
}

// Проверял на двух устройствах - всё работает
int main()
{
    int fd;
    struct sockaddr_ll sa;

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    // Получаем индекс интерфейса
    int ifindex = if_nametoindex(INTERFACE);
    if(ifindex == 0)
    {
        perror("if_nametoindex error!\n");
        exit(EXIT_FAILURE);
    }

    // Получаем MAC адрес интерфейса
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, INTERFACE);
    if(ioctl(fd, SIOCGIFHWADDR, &ifr) == -1)
    {
        perror("Ioctl hwaddr error!\n");
        exit(EXIT_FAILURE);
    }
    uint8_t *source_mac = (uint8_t*)ifr.ifr_hwaddr.sa_data;

    // Заполняем sockaddr_ll
    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = htons(ETH_P_IP);
    sa.sll_ifindex = ifindex;
    sa.sll_halen = 6;
    memcpy(sa.sll_addr, source_mac, 6);

    // Получаем строку от клиента
    printf("Введите сообщение: ");
    char msg[256];
    fgets(msg, sizeof(msg), stdin);
    msg[strlen(msg) - 1] = '\0'; // Заменяем \n на ничего

    int udp_len = sizeof(struct udpheader) + strlen(msg);
    int ip_len = sizeof(struct ipheader) + udp_len;
    int total_len = sizeof(struct ethheader) + ip_len;

    // Заполняем udp заголовок
    struct udpheader udp;
    udp.source_port = htons(CLIENT_PORT);
    udp.destination_port = htons(SERVER_PORT);
    udp.length = htons(udp_len);
    udp.check_sum = 0;

    // Заполняем ip заголовок
    struct ipheader ip;
    ip.version_ihl = 0x45;
    ip.tos = 0;
    ip.total_length = htons(ip_len);
    ip.id = 0;
    ip.flags_offset = 0;
    ip.ttl = 64;
    ip.protocol = 17;
    ip.checksum = 0;
    inet_pton(AF_INET, "192.168.0.40", &ip.source_ip); // Ставлю ip источника - ip моего ноутбука
    inet_pton(AF_INET, SERVER_IP, &ip.dest_ip);
    ip.checksum = calculate_checksum((uint16_t*)&ip, sizeof(struct ipheader));

    // Заполняем ethernet заголовок
    struct ethheader eth;
    memcpy(eth.source_mac, source_mac, 6);
    // Чтобы не вводить mac адрес моего компьютера, отправляю всем устройствам в сети по широковещательной рассылке
    uint8_t broadcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
    memcpy(eth.dest_mac, broadcast_mac, 6);
    eth.ethertype = htons(ETH_P_IP);

    // Заполняем буффер с ethernet заголовком + ip заголовком + udp заголовоком + наша строка
    char buffer[total_len];
    memcpy(buffer, &eth, sizeof(struct ethheader));
    memcpy(buffer + sizeof(struct ethheader), &ip, sizeof(struct ipheader));
    memcpy(buffer + sizeof(struct ethheader) + sizeof(struct ipheader), &udp, sizeof(struct udpheader));
    memcpy(buffer + sizeof(struct ethheader) + sizeof(struct ipheader) + sizeof(struct udpheader), msg, strlen(msg));

    // Отправляем данные серверу
    if(sendto(fd, buffer, total_len, 0, (struct sockaddr*)&sa, sizeof(sa)) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Строка отправлена!\n");

    // Принимаем данные
    while(1)
    {
        char recv_buffer[2048];
        socklen_t sa_len = sizeof(sa);
        int num_bytes = recvfrom(fd, recv_buffer, sizeof(recv_buffer) - 1, 0, (struct sockaddr*)&sa, &sa_len);
        if(num_bytes == -1)
        {
            perror("Recv error!\n");
            exit(EXIT_FAILURE);
        }
        // Получаем udp заголовок, пропуская ethernet 14 байт + ip 20 байт
        struct udpheader *recv_udp = (struct udpheader*)(recv_buffer + 14 + 20);

        if(ntohs(recv_udp->destination_port) == CLIENT_PORT)
        {
            char *data = recv_buffer + 14 + 20 + 8; // Получаем payload, пропуская 14 байт ethernet + 20 байт ip + 8 байт заголовка udp
            data[num_bytes - 14 - 20 - 8] = '\0'; // Добавляем в конец сообщения '\0'
            printf("Получено: %s\n", data);
            exit(EXIT_SUCCESS);
        }
        else continue;
    }

    close(fd);
    exit(EXIT_SUCCESS);
}