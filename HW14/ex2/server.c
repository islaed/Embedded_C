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

// Отправка сообщения клиенту
void send_to_client(int client_shm_id, int client_sem_id, msgbuf *message)
{
    // Подключаемся к памяти клиента
    message_buffer *client_buf = (message_buffer *)shmat(client_shm_id, NULL, 0);
    if(client_buf == (void *)-1)
    {
        printf("Ошибка подключения к памяти клиента!\n");
        return;
    }

    // Ждем освобождения места в буфере клиента
    sem_wait_custom(client_sem_id, 1); // Семафор пустых слотов

    // Записываем сообщение
    client_buf->messages[client_buf->write_pos] = *message;
    client_buf->write_pos = (client_buf->write_pos + 1) % MAX_MESSAGES;
    client_buf->count++;

    // Сигнализируем о новом сообщении
    sem_post_custom(client_sem_id, 0); // Семафор заполненных слотов

    shmdt(client_buf);
}

// Отправка сообщения всем клиентам
void message_send(client_list *client_list, int mtype, msgbuf message_rcv)
{
    msgbuf message_snd;
    message_snd.mtype = mtype;

    switch(mtype)
    {
        case MSG_LIST:
            printf("===== Приоритет 2 =====\n");
            int clients_count = get_clients_list(client_list, &message_snd);
            printf("Отправляем список пользователей:\n");
            for(int i = 0; i < clients_count; i++)
            {
                printf("%s\n", message_snd.auth_clients[i]);
            }
            message_snd.clients_count = clients_count;

            for(client_node *current_client = client_list->head; current_client != NULL; current_client = current_client->next)
            {
                // Получаем семафоры клиента
                key_t client_key = ftok(Q_MAIN, current_client->client_pid);
                int client_sem_id = semget(client_key, 2, 0);
                if(client_sem_id == -1)
                {
                    printf("Ошибка получения семафоров для %s!\n", current_client->name);
                    continue;
                }
                send_to_client(current_client->client_shm_id, client_sem_id, &message_snd);
            }
            break;

        case MSG_BROADCAST:
            printf("===== Приоритет 4 =====\n");
            strcpy(message_snd.name, message_rcv.name);
            strcpy(message_snd.text, message_rcv.text);
            printf("Отправлено: [%s]: %s\n", message_snd.name, message_snd.text);

            for(client_node *current_client = client_list->head; current_client != NULL; current_client = current_client->next)
            {
                key_t client_key = ftok(Q_MAIN, current_client->client_pid);
                int client_sem_id = semget(client_key, 2, 0);
                if(client_sem_id == -1)
                {
                    printf("Ошибка получения семафоров для %s!\n", current_client->name);
                    continue;
                }
                send_to_client(current_client->client_shm_id, client_sem_id, &message_snd);
            }
            break;
    }
}

// Получение сообщения от клиента
void message_receive(client_list *client_list, message_buffer *server_buf, int server_sem_id)
{
    // Ждем сообщения
    sem_wait_custom(server_sem_id, 0); // Семафор заполненных слотов

    // Читаем сообщение
    msgbuf message_rcv = server_buf->messages[server_buf->read_pos];
    server_buf->read_pos = (server_buf->read_pos + 1) % MAX_MESSAGES;
    server_buf->count--;

    // Освобождаем слот
    sem_post_custom(server_sem_id, 1); // Семафор пустых слотов

    switch(message_rcv.mtype)
    {
        case MSG_AUTH:
            printf("===== Приоритет 1 =====\n");
            add_client(client_list, message_rcv);
            printf("Подключился: %s\n", message_rcv.name);
            message_send(client_list, MSG_LIST, message_rcv);
            break;
        case MSG_TEXT:
            printf("===== Приоритет 3 =====\n");
            printf("Получено: [%s]: %s\n", message_rcv.name, message_rcv.text);
            message_send(client_list, MSG_BROADCAST, message_rcv);
            break;
        case MSG_DEAUTH:
            printf("===== Приоритет 5 =====\n");
            printf("Удаление клиента: %s\n", message_rcv.name);
            remove_client(client_list, message_rcv);
            message_send(client_list, MSG_LIST, message_rcv);
            break;
    }
}

int main()
{
    client_list clients;
    init_list(&clients);

    // Создаём ключ для сервера
    key_t key_main = ftok(Q_MAIN, 0);
    if(key_main == -1)
    {
        printf("Ошибка создания ключа!\n");
        return 1;
    }

    // Создаём разделяемую память для сервера
    int server_shm_id = shmget(key_main, sizeof(message_buffer), IPC_CREAT | 0666);
    if(server_shm_id == -1)
    {
        printf("Ошибка создания разделяемой памяти!\n");
        return 1;
    }

    // Подключаем разделяемую память
    message_buffer *server_buf = (message_buffer *)shmat(server_shm_id, NULL, 0);
    if(server_buf == (void *)-1)
    {
        printf("Ошибка подключения памяти!\n");
        return 1;
    }

    // Инициализируем буфер
    server_buf->read_pos = 0;
    server_buf->write_pos = 0;
    server_buf->count = 0;

    // Создаём семафоры (0 - заполненные слоты, 1 - пустые слоты)
    int server_sem_id = semget(key_main, 2, IPC_CREAT | 0666);
    if(server_sem_id == -1)
    {
        printf("Ошибка создания семафоров!\n");
        return 1;
    }

    // Инициализируем семафоры
    union semun arg;
    arg.val = 0;
    semctl(server_sem_id, 0, SETVAL, arg); // Заполненные слоты = 0
    arg.val = MAX_MESSAGES;
    semctl(server_sem_id, 1, SETVAL, arg); // Пустые слоты = MAX_MESSAGES

    printf("Сервер запущен\n");

    // Бесконечный цикл обработки сообщений
    while(1)
    {
        message_receive(&clients, server_buf, server_sem_id);
    }

    return 0;
}
