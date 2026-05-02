#include "dlist.h"

#define Q_MAIN "."

/*
Получаем
    mtype == 1 - авторизируем клиента на сервере
    mtype == 3 - получаем обычное сообщение от клиента
    mtype == 5 - деавторизируем клиента на сервере
Отправляем
    mtype == 2 - отправляем клиентам список авторизованных на сервере
    mtype == 4 - отправляем сообщение всем клиентам в чат
*/

// Отправка сообщения
void message_send(client_list *client_list, int mtype, msgbuf message_rcv)
{
    int msg_snd;
    int client_msgid;
    msgbuf message_snd;
    message_snd.mtype = mtype;
    // printf("Отправка сообщения! mtype = %d\n", mtype);

    // Проверяем mtype
    switch(mtype)
    {
        case 2:
            // Если mtype == 2, то получаем список клиентов и отправляем его
            printf("===== Приоритет 2 =====\n");
            int clients_count = get_clients_list(client_list, &message_snd);
            printf("Список пользователей:\n");
            for(int i = 0; i < clients_count; i++)
            {
                printf("%s\n", message_snd.auth_clients[i]);
            }
            message_snd.clients_count = clients_count;
            for(client_node *current_client = client_list->head; current_client != NULL; current_client = current_client->next)
            {
                client_msgid = msgget(current_client->client_key, 0666);
                msg_snd = msgsnd(client_msgid, &message_snd, sizeof(message_snd) - sizeof(long), IPC_NOWAIT);
                if(msg_snd == -1)
                {
                    printf("Ошибка отправки сообщения!\n");
                    return;
                }
            }
            break;
        
        case 4:
            // Если mtype == 4, то создаём сообщение с именем и текстом и отправляем его
            printf("===== Приоритет 4 =====\n");
            strcpy(message_snd.name, message_rcv.name);
            strcpy(message_snd.text, message_rcv.text);
            printf("Отправлено: [%s]: %s\n", message_snd.name, message_snd.text);


            for(client_node *current_client = client_list->head; current_client != NULL; current_client = current_client->next)
            {
                client_msgid = msgget(current_client->client_key, 0666);
                msg_snd = msgsnd(client_msgid, &message_snd, sizeof(message_snd) - sizeof(long), IPC_NOWAIT);
                if(msg_snd == -1)
                {
                    printf("Ошибка отправки сообщения!\n");
                    return;
                }
            }
            break;
    }
}

// Получение сообщения
void message_recieve(client_list *client_list, int qid)
{
    msgbuf message_rcv; // Получаемое сообщение
    message_rcv.mtype = 0; // Ставим приоритет 0, чтобы получать все

    // Читаем сообщения
    ssize_t message_read = msgrcv(qid, &message_rcv, sizeof(msgbuf) - sizeof(long), message_rcv.mtype, 0);
    if(message_read == -1)
    {
        printf("Ошибка чтения сообщения!\n");
        return;
    }
    // printf("Приоритет: %ld, pid: %d, key: %ld, имя: %s, текст: %s\n",
    //     message_rcv.mtype,
    //     message_rcv.client_pid,
    //     (long)message_rcv.client_key,
    //     message_rcv.name,
    //     message_rcv.text
    // );
    
    switch(message_rcv.mtype)
    {
        case 1:
            // Если mtype == 1, то записываем данные клиента в список и осведомляем остальных
            printf("===== Приоритет 1 =====\n");
            add_client(client_list, message_rcv);
            printf("Подключился: %s\n", message_rcv.name);
            message_send(client_list, 2, message_rcv);
            break;
        case 3:
            // Если mtype == 3, то отправляем сообщение с текстом и именем всем клиентам
            printf("===== Приоритет 3 =====\n");
            printf("Получено: [%s]: %s\n", message_rcv.name, message_rcv.text);
            message_send(client_list, 4, message_rcv);
            break;
        case 5:
            // Если mtype == 5, то удаляем клиента из списка и осведомляем остальных
            printf("===== Приоритет 5 =====\n");
            printf("Удаление клиента: %s\n", message_rcv.name);
            remove_client(client_list, message_rcv);
            message_send(client_list, 2, message_rcv);
            break;
    }
}


int main()
{
    client_list clients; // Список клиентов
    init_list(&clients);

    // Создаём ключ основной очереди
    key_t key_main;
    key_main = ftok(Q_MAIN, 0);
    if(key_main == -1)
    {
        printf("Ошибка!\n");
        return 1;
    }
    // Создаём очередь для авторизации
    // IPC_CREAT создаёт файл, 0666 права доступа к файлу
    int qid = msgget(key_main, IPC_CREAT | 0666);
    if(qid == -1)
    {
        printf("Ошибка открытия очереди!\n");
        return 1;
    }

    // Бесконечный цикл, чтобы сервер всегда ожидал сообщений от клиентов
    while(1)
    {
        message_recieve(&clients, qid);
    }
    

    return 0;
}