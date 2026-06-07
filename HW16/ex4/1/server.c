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


void change_random_symbol(char *changing_string)
{
    int str_len = strlen(changing_string);
    if(str_len == 0) return;
    int rand_index = rand() % str_len;
    changing_string[rand_index] = 'A';
}

int main()
{
    srand(time(NULL));
    int fd;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
    server.sin_port = htons(SERVER_PORT);

    if(bind(fd, (struct sockaddr*)&server, sizeof(server)) == -1)
    {
        perror("Bind error!\n");
        exit(EXIT_FAILURE);
    }
    
    // Принимаем строку от клиента
    char buf[256];
    int num_bytes = recvfrom(fd, buf,sizeof(buf) - 1, 0,
            (struct sockaddr *) &client, &client_len);
    if(num_bytes == -1)
    {
        perror("Recv error!\n");
        exit(EXIT_FAILURE); 
    }
    buf[num_bytes] = '\0';
    printf("Получено: %s\n", buf);

    change_random_symbol(buf);
    if(sendto(fd, buf, strlen(buf), 0, (struct sockaddr*)&client, client_len) == -1)
    {
        perror("Send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено клиенту: %s\n", buf);

    close(fd);
    exit(EXIT_SUCCESS);
}