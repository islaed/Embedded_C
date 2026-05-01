#include "dlist.h"

// Инициализация пустого списка клиентов
void init_list(client_list *list)
{
    list->head = NULL;
    list->tail = NULL;
}

// Добавление клиента
void add_client(client_list *list, msgbuf message_rcv)
{
    // Выделяем память под нового клиента
    client_node *new_client = (client_node*)malloc(sizeof(client_node));

    // Записываем данные клиента
    new_client->client_pid = message_rcv.client_pid;
    new_client->client_key = message_rcv.client_key;
    strcpy(new_client->name, message_rcv.name);

    new_client->next = NULL;
    new_client->prev = list->tail;

    if(list->tail == NULL) // Если список пуст
    {
        list->head = new_client;
        list->tail = new_client;
    }
    else
    {
        list->tail->next = new_client;
        list->tail = new_client;
    }
}

// Удаление клиента
void remove_client(client_list *list, msgbuf message_rcv)
{
    client_node *current_client = list->head;
    while(current_client != NULL)
    {
        client_node *next_client = current_client->next; // Сохраняем ссылку на следующий

        if(current_client->client_pid == message_rcv.client_pid)
        {
            if(current_client->prev != NULL)
                current_client->prev->next = current_client->next;
            else
                list->head = current_client->next; // Удаляем первый элемент

            if(current_client->next != NULL)
                current_client->next->prev = current_client->prev;
            else
                list->tail = current_client->prev; // Удаляем последний элемент

            free(current_client);
        }
        current_client = next_client;
    }
}

// Получение списка имени клиентов
void get_clients_list(client_list *list, msglist *message_list)
{
    int counter = 0;
    for(client_node *current_client = list->head; current_client != NULL; current_client = current_client->next)
    {
        strcpy(message_list->auth_clients[counter], current_client->name);
        counter++;
    }
}
