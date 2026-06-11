#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <netinet/in.h> // Для типов uint32 и uint16

// Информация о водителе
typedef struct client_node {
    uint32_t client_ip;
    uint16_t client_port;
    int message_id;
    int client_id;
    struct client_node *next;
} client_node_t;

// Список водителей
typedef struct {
    client_node_t *head;
} client_list_t;

// Инициализация пустого списка
void client_list_init(client_list_t *list);

// Существует ли клиент с такими данными
int client_list_exists(client_list_t *list, uint32_t client_ip, uint16_t client_port);

// Добавить водителя в список
int client_list_add(client_list_t *list, uint32_t client_ip, uint16_t client_port, int client_id);

// Удаление клиента из списка
int client_list_remove(client_list_t *list, uint32_t client_ip, uint16_t client_port);

// Получить client id
int client_list_get_client_id(client_list_t *list, uint32_t client_ip, uint16_t client_port);

// Получить message_id клиента
int client_list_get_message_id(client_list_t *list, uint32_t client_ip, uint16_t client_port);

// Добавить 1 к message_id клиента
int client_list_increment_message_id(client_list_t *list, uint32_t client_ip, uint16_t client_port);



#endif
