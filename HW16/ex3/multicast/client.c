#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MULTICAST_IP "224.0.0.1"
#define PORT 15000


int main()
{
    int fd;
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = htons(PORT);

    if(bind(fd, (struct sockaddr*) &client, sizeof(client)) == -1)
    {
        perror("Bind error!\n");
        exit(EXIT_FAILURE);
    }

    // Подключаемся к мультикаст группе
    struct ip_mreqn mreqn;
    memset(&mreqn, 0, sizeof(mreqn));
    inet_pton(AF_INET, MULTICAST_IP, &mreqn.imr_multiaddr);
    mreqn.imr_address.s_addr = htonl(INADDR_ANY);
    mreqn.imr_ifindex = 0;

    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn));


    // Получаем мультикастовое сообщение
    char buf[256];
    int bytes = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &client_len);
    if(bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE);
    }
    buf[bytes] = '\0';
    printf("Получено: %s\n", buf);

    
    close(fd);
    exit(EXIT_SUCCESS);
}