#include <stdio.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define Q_NAME "."

/*
Структура сервера

Две очереди: одна от клиентов, вторая к клиентам

Получаем сообщение, если приоритет mtype == 1, значит получаем имя клиента
и записываем его имя на сервере.
Рассылаем всем сообщение с приоритетом 2, чтобы клиенты у себя увидели других клиентов

Если приоритет mtype == 3, значит обычное сообщение для всех пользователей.
Рассылаем всем клиентам в чат сообщение с приоритетом 4
*/





// Структура сообщения
typedef struct msgbuf {
    long mtype;
    char mtext[256];
} msgbuf;

void message_recieve(msgbuf message_rcv, int qid)
{
    // Получаем сообщение, но с приоритетом 1, с которым нам отправляет клиент
    message_rcv.mtype = 1;
    ssize_t message_read = msgrcv(qid, &message_rcv, sizeof(message_rcv.mtext), message_rcv.mtype, 0);
    if(message_read == -1)
    {
        printf("Ошибка чтения сообщения!\n");
        return;
    }
    printf("Получено: %s\n", message_rcv.mtext);
    return;
}

void user_auth()
{
    while(1)
    {

    }
    return;
}


int main()
{
    struct msgbuf message_rcv;
    int qid;
    
    // Создаём ключ
    key_t key;
    key = ftok(Q_NAME, 0);
    if(key == -1)
    {
        printf("Ошибка!\n");
        return 1;
    }

    // Создаём очередь
    qid = msgget(key, IPC_CREAT | 0666); // IPC_CREAT создаёт файл, 0666 права доступа к файлу
    if(qid == -1)
    {
        printf("Ошибка открытия очереди!\n");
        return 1;
    }

    while(1)
    {
        message_recieve(message_rcv, qid);
    }

    return 0;
}