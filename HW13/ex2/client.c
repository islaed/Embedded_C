#include "dlist.h"
#include <pthread.h>
#include <unistd.h>

#define Q_MAIN "."

int main_qid;
int client_qid;
char name[MAX_NAME_SIZE];
pid_t pid;

void *message_send(void *args)
{
    char message[MAX_TEXT_SIZE];
    msgbuf message_snd;

    while(1)
    {
        printf("Напишите сообщение: ");
        scanf("%s", message);


        if(strcmp(message, "exit") == 0)
        {
            message_snd.mtype = 5;
            message_snd.client_pid = pid;
        }
        else
        {
            message_snd.mtype = 3;
            strcpy(message_snd.text, message);
        }
        strcpy(message_snd.name, name);

        int msg_snd = msgsnd(main_qid, &message_snd, sizeof(message_snd) - sizeof(long), IPC_NOWAIT);
        if(msg_snd == -1)
        {
            // printf("Ошибка отправки сообщения!\n");
            exit(0);
        }
        if(message_snd.mtype == 5)
        {
            exit(0);
        }
    }
}

void *message_recieve(void *args)
{
    msgbuf message_rcv; // Получаемое сообщение
    
    while(1)
    {
        ssize_t message_read = msgrcv(client_qid, &message_rcv, sizeof(msgbuf) - sizeof(long), 0, 0);
        if(message_read == -1)
        {
            // printf("Ошибка чтения сообщения!\n");
            exit(0);
        }
        switch(message_rcv.mtype)
        {
            case 2: // Получаем список клиентов на сервере
                for(int i = 0; i < message_rcv.clients_count; i++)
                {
                    printf("%s\n", message_rcv.auth_clients[i]);
                }
                break;
            
            case 4: // Получаем сообщение от клиента
                char message_text[MAX_NAME_SIZE + MAX_TEXT_SIZE + 4]; // +4 для символов []: и пробела
                snprintf(message_text, sizeof(message_text), "[%s]: %s", message_rcv.name, message_rcv.text);
                printf("%s\n", message_text);
                break;
        }
    }
}

int main()
{
    pid = getpid();
    key_t key_client;
    key_client = ftok(Q_MAIN, pid);
    
    printf("Введите своё имя: ");
    scanf("%s", name);
    
    msgbuf client_data;
    client_data.mtype = 1;
    client_data.client_key = key_client;
    client_data.client_pid = pid;
    strcpy(client_data.name, name);
    
    // Создаём личную очередь для клиента
    client_qid = msgget(key_client, IPC_CREAT | 0666);
    if(client_qid == -1)
    {
        printf("Ошибка открытия личной очереди!\n");
        return 1;
    }

    // Подключаемся к очереди сервера
    key_t key_main;
    key_main = ftok(Q_MAIN, 0);
    main_qid = msgget(key_main, 0);
    if(main_qid == -1)
    {
        printf("Ошибка открытия общей очереди!\n");
        return 1;
    }

    // Отправляем свои данные на сервер
    int msg_snd = msgsnd(main_qid, &client_data, sizeof(client_data) - sizeof(long), IPC_NOWAIT);
    if(msg_snd == -1)
    {
        printf("Ошибка отправки сообщения!\n");
        return 1;
    }

    pthread_t recieve;
    pthread_t send;
    
    pthread_create(&recieve, NULL, &message_recieve, NULL);
    pthread_create(&send, NULL, &message_send, NULL);

    pthread_join(recieve, NULL);
    pthread_join(send, NULL);
     
    return 0;
}