#include "client_list.h"
#include <stdlib.h>

// Локальная функция, которая видна только в этом файле
static client_node_t* find_client(client_list_t *list, uint32_t client_ip, uint16_t client_port)
{
    client_node_t *current = list->head;
    while(current != NULL)
    {
        if(current->client_ip == client_ip && current->client_port == client_port)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Инициализация пустого списка
void client_list_init(client_list_t *list)
{
    list->head = NULL;
}

// Существует такой клиент или нет
int client_list_exists(client_list_t *list, uint32_t client_ip, uint16_t client_port)
{
    if(find_client(list, client_ip, client_port) == NULL)
    {
        return 0; // Клиент не существует
    }

    return 1; // Клиент существует
}

// Добавить клиента в список
int client_list_add(client_list_t *list, uint32_t client_ip, uint16_t client_port, int client_id)
{
    client_node_t *new_node = (client_node_t*)malloc(sizeof(client_node_t));
    if(new_node == NULL)
    {
        return -1;
    }
    
    new_node->client_ip = client_ip;
    new_node->client_port = client_port;
    new_node->message_id = 1;
    new_node->client_id = client_id;
    new_node->next = list->head;
    list->head = new_node;

    return 0;
}

// Удаление клиента из списка
int client_list_remove(client_list_t *list, uint32_t client_ip, uint16_t client_port)
{
    client_node_t *client = find_client(list, client_ip, client_port);
    if(client == NULL)
    {
        return -1; // Клиент не найден
    }
    
    if(list->head == client)
    {
        list->head = client->next;
    }
    else
    {
        client_node_t *current = list->head;
        while(current->next != client)
        {
            current = current->next;
        }
        current->next = client->next;
        free(client);
    }
    return 0;
}

// Получить client id
int client_list_get_client_id(client_list_t *list, uint32_t client_ip, uint16_t client_port)
{
    client_node_t *client = find_client(list, client_ip, client_port);
    if(client == NULL)
    {
        return -1; // Клиент не найден
    }
    return client->client_id;
}

// Получить message id клиента
int client_list_get_message_id(client_list_t *list, uint32_t client_ip, uint16_t client_port)
{
    client_node_t *client = find_client(list, client_ip, client_port);
    if(client == NULL)
    {
        return -1; // Клиент не найден
    }
    return client->message_id;
}

// Добавить 1 к message id клиента
int client_list_increment_message_id(client_list_t *list, uint32_t client_ip, uint16_t client_port)
{
    client_node_t *client = find_client(list, client_ip, client_port);
    if(client == NULL)
    {
        return -1; // Клиент не найден
    }
    
    client->message_id++;
    
    return 0;
}