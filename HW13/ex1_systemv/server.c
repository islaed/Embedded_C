#include <stdio.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// Структура сообщения
struct msgbuf {
    long mtype;
    char mtext[256];
};

int main()
{
    const char *q_name = "."; // Путь к папке, где будет очередь
    const char *message_send = "Hi!"; // Передаваемое сообщение

    int qid;
    int msg_snd;

    // Структура передаваемого сообщения
    struct msgbuf msg_send;
    msg_send.mtype = 1; // Приоритет сообщения
    strcpy(msg_send.mtext, message_send);

    // Структура принимаемого сообщения
    struct msgbuf msg_recieve;
    
    // Создаём ключ
    key_t key;
    key = ftok(q_name, 0);
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

    // Отправляем сообщение с приоритетом 1
    msg_snd = msgsnd(qid, &msg_send, sizeof(msg_send.mtext), IPC_NOWAIT);
    if(msg_snd == -1)
    {
        printf("Ошибка отправки сообщения!\n");
        return 1;
    }

    // Получаем сообщение, но с приоритетом 2, с которым нам отправляет клиент
    ssize_t message_read = msgrcv(qid, &msg_recieve, sizeof(msg_recieve.mtext), 2, 0);
    if(message_read == -1)
    {
        printf("Ошибка чтения сообщения!\n");
        return 1;
    }
    printf("Получено: %s\n", msg_recieve.mtext);

    return 0;
}