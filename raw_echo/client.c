#define _GNU_SOURCE // без него siginfo_t, sigset_t и прочие подчеркиваются как ошибки
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 15000


// Создаём структуру udp заголовка, каждое поле по 2 байта
struct udpheader{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t check_sum;
};

// Выносим эти переменные в глобальные, чтобы была возможность потом отправить серверу
// сообщение о закрытии
int fd;
struct sockaddr_in server;
socklen_t server_len;
struct udpheader udp;

void sig_handler(int sig_num)
{
    printf("Handler!!!\n");
    char *msg = "EXIT";
    int total_len = sizeof(struct udpheader) + strlen(msg);
    udp.length = htons(total_len);
    
    // Заполняем буффер с udp заголовоком + наша строка
    char buffer[total_len];
    memcpy(buffer, &udp, sizeof(struct udpheader));
    memcpy(buffer + sizeof(struct udpheader), msg, strlen(msg));
    
    sendto(fd, buffer, total_len, 0, (struct sockaddr*)&server, sizeof(server));
    exit(EXIT_SUCCESS);
}

int main()
{
    struct sigaction handler;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    handler.sa_handler = sig_handler;
    handler.sa_mask = set;

    if(sigaction(SIGINT, &handler, NULL) == -1)
    {
        perror("Cant set signal handler!\n");
        exit(EXIT_FAILURE);
    }

    int get_port_fd; // get_port_fd нужен для того, чтобы занять порт в системе
    struct sockaddr_in server, client_port;
    socklen_t server_len = sizeof(server);
    socklen_t client_port_len = sizeof(client_port);
    
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(SERVER_PORT);

    // Занимаем порт в системе
    get_port_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(get_port_fd == -1)
    {
        perror("Socket1 error!\n");
        exit(EXIT_FAILURE);
    }
    client_port.sin_family = AF_INET;
    client_port.sin_addr.s_addr = INADDR_ANY;
    client_port.sin_port = htons(0); // Изза 0 система выдаёт нам свободный порт
    if(bind(get_port_fd, (struct sockaddr*)&client_port, sizeof(client_port)) == -1)
    {
        perror("Bind error!\n");
        exit(EXIT_FAILURE);
    }
    // Забираем номер порта, чтобы потом закинуть его в udp заголовок
    if(getsockname(get_port_fd, (struct sockaddr*)&client_port, &client_port_len) == -1)
    {
        perror("Getsockname error!\n");
        exit(EXIT_FAILURE);
    }
    
    // Заполняем udp заголовок
    udp.source_port = client_port.sin_port;
    udp.destination_port = htons(SERVER_PORT);
    udp.check_sum = 0;
    
    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1)
    {
        perror("Socket2 error!\n");
        exit(EXIT_FAILURE);
    }
    

    while(1)
    {
        // Получаем строку от клиента
        printf("Введите сообщение: ");
        char msg[256];
        fgets(msg, sizeof(msg), stdin);
        msg[strlen(msg) - 1] = '\0'; // Заменяем \n на ничего
        
        int total_len = sizeof(struct udpheader) + strlen(msg);
        udp.length = htons(total_len);
        
        // Заполняем буффер с udp заголовоком + наша строка
        char buffer[total_len];
        memcpy(buffer, &udp, sizeof(struct udpheader));
        memcpy(buffer + sizeof(struct udpheader), msg, strlen(msg));

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
            if(recv_udp->destination_port == udp.source_port)
            {
                char *data = recv_buffer + 20 + 8; // Получаем payload, пропуская 20 байт ip + 8 байт заголовка udp
                data[num_bytes - 20 - 8] = '\0'; // Добавляем в конец сообщения '\0'
                printf("Получено: %s\n", data);
                break;
            }
            else continue;
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
}