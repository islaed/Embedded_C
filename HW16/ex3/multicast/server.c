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
    struct sockaddr_in clients;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    memset(&clients, 0, sizeof(clients));
    clients.sin_family = AF_INET;
    inet_pton(AF_INET, MULTICAST_IP, &clients.sin_addr);
    clients.sin_port = htons(PORT);

    // Отправляем сообщение
    char *msg = "Multicast message!";
    if(sendto(fd, msg, strlen(msg), 0, (struct sockaddr *) &clients, sizeof(clients)) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Broadcast сообщение успешно отправлено!\n");

    close(fd);
    exit(EXIT_SUCCESS);
}