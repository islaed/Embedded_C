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
void message_send(client_list *client_list, int mtype, char name[MAX_NAME_SIZE], char text[MAX_TEXT_SIZE])
{
    int msg_snd;

    // Проверяем mtype
    switch(mtype)
    {
        case 2:
            // Если mtype == 2, то получаем список клиентов и отправляем его
            msglist list;
            list.mtype = mtype;
            get_clients_list(client_list, &list);
            for(client_node *current_client = client_list->head; current_client != NULL; current_client = current_client->next)
            {
                msg_snd = msgsnd(current_client->client_key, &list, sizeof(list) - sizeof(long), IPC_NOWAIT);
                if(msg_snd == -1)
                {
                    printf("Ошибка отправки сообщения!\n");
                    return;
                }
            }
            break;
        
        case 4:
            // Если mtype == 4, то создаём сообщение с именем и текстом и отправляем его
            msgbuf message_snd;
            message_snd.mtype = mtype;
            strcpy(message_snd.name, name);
            strcpy(message_snd.text, text);

            for(client_node *current_client = client_list->head; current_client != NULL; current_client = current_client->next)
            {
                msg_snd = msgsnd(current_client->client_key, &message_snd, sizeof(message_snd) - sizeof(long), IPC_NOWAIT);
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
    printf("Приоритет: %ld, pid: %d, key: %ld, имя: %s, текст: %s\n", message_rcv.mtype, message_rcv.client_pid, (long)message_rcv.client_key,
        message_rcv.name, message_rcv.text);
    switch(message_rcv.mtype)
    {
        case 1:
            // Если mtype == 1, то записываем данные клиента в список и осведомляем остальных
            add_client(client_list, message_rcv);
            printf("Подключился: %s\n", message_rcv.name);
            message_send(client_list, 2, NULL, NULL);
            break;
        case 3:
            // Если mtype == 3, то отправляем сообщение с текстом и именем всем клиентам
            printf("Сообщение: [%s] %s\n", message_rcv.name, message_rcv.text);
            message_send(client_list, 4, message_rcv.name, message_rcv.text);
            break;
        case 5:
            // Если mtype == 5, то удаляем клиента из списка и осведомляем остальных
            remove_client(client_list, message_rcv);
            message_send(client_list, 2, NULL, NULL);
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