#include <stdio.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define Q_NAME "."

typedef struct msgbuf {
    long mtype;
    char mtext[256];
} msgbuf;

// Отправка сообщения
void message_send(int qid, msgbuf message_snd)
{
    if(message_snd.mtype == 1)
    {
        if(msgsnd(qid, &message_snd, sizeof(message_snd.mtext), IPC_NOWAIT) == -1)
        {
            printf("Ошибка отправки сообщения!\n");
            return;
        }
    }
    else
    {
        message_snd.mtype = 2;
        if(msgsnd(qid, &message_snd, sizeof(message_snd.mtext), IPC_NOWAIT) == -1)
        {
            printf("Ошибка отправки сообщения!\n");
            return;
        }
    }
}

// Авторизация на сервере
void login(int qid, char *client_name)
{
    msgbuf message_snd;
    message_snd.mtype = 1;
    strcpy(message_snd.mtext, client_name);

    message_send(qid, message_snd);
}

int main()
{
    int qid;
    char name[256];

    key_t key;
    key = ftok(Q_NAME, 0);
    if(key == -1)
    {
        printf("Ошибка!\n");
        return 1;
    }
    qid = msgget(key, 0);
    if(qid == -1)
    {
        printf("Ошибка открытия очереди!\n");
        return 1;
    }

    printf("Введите своё имя: ");
    while(1)
    {
        scanf("%s", name);
        login(qid, name);
    }

    return 0;
}