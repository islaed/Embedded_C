#include <stdio.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct msgbuf {
    long mtype;
    char mtext[256];
};

int main()
{
    const char *q_name = ".";
    const char *message_send = "Hello!";

    int qid;
    int msg_snd;

    // Структура получаемого сообщения
    struct msgbuf msg_recieve;

    // Структура отправляемого сообщения
    struct msgbuf msg_send;
    msg_send.mtype = 2;
    strcpy(msg_send.mtext, message_send);
    
    key_t key;
    key = ftok(q_name, 0);
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

    // Принимаем сообщение с приоритетом 1, с которым нам отправил сервер
    ssize_t message_read = msgrcv(qid, &msg_recieve, sizeof(msg_recieve.mtext), 1, 0);
    if(message_read == -1)
    {
        printf("Ошибка чтения сообщения!\n");
        return 1;
    }
    printf("Получено: %s\n", msg_recieve.mtext);

    // Отправляем сообщение с приоритетом 2 для сервера
    msg_snd = msgsnd(qid, &msg_send, sizeof(msg_send.mtext), IPC_NOWAIT);
    if(msg_snd == -1)
    {
        printf("Ошибка отправки сообщения!\n");
        return 1;
    }

    return 0;
}