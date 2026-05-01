#include "dlist.h"
#include <pthread.h>
#include <unistd.h>

#define Q_MAIN "."

int main()
{
    key_t key_main;
    key_main = ftok(Q_MAIN, 0);

    pid_t pid = getpid();
    key_t key_client;
    key_client = ftok(Q_MAIN, pid);

    char name[MAX_NAME_SIZE];
    printf("Введите своё имя: ");
    scanf("%s", name);

    msgbuf client;
    client.mtype = 1;
    client.client_key = key_client;
    client.client_pid = pid;
    strcpy(client.name, name);

    int qid = msgget(key_main, 0);
    if(qid == -1)
    {
        printf("Ошибка открытия очереди!\n");
        return 1;
    }

    int msg_snd = msgsnd(qid, &client, sizeof(client) - sizeof(long), IPC_NOWAIT);
    if(msg_snd == -1)
    {
        printf("Ошибка отправки сообщения!\n");
        return 1;
    }

    return 0;
}