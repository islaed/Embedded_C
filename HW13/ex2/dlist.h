#ifndef DLIST_H
#define DLIST_H

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

#define MAX_NAME_SIZE 32 // Максимальный размер имени
#define MAX_TEXT_SIZE 256 // Максимальный размер текста в сообщении
#define MAX_CLIENTS 32



// Структура для отправки сообщений
typedef struct msgbuf
{
    long mtype;

    pid_t client_pid; // pid клиента
    key_t client_key; // Ключ клиента

    char name[MAX_NAME_SIZE]; // Имя клиента
    char text[MAX_TEXT_SIZE]; // Текст сообщения клиента
    char auth_clients[MAX_CLIENTS][MAX_NAME_SIZE]; // Список клиентов
    int clients_count; // Количество подключенных клиентов
} msgbuf;

// Структура клиента для двусвязного списка
typedef struct client_node
{
    struct client_node *next;
    struct client_node *prev;

    pid_t client_pid; // pid клиента
    key_t client_key; // Ключ клиента

    char name[MAX_NAME_SIZE]; // Имя клиента
} client_node;

// Указатели на первый и последний элемент в двусвязном списке
typedef struct client_list
{
    client_node *head;
    client_node *tail;
} client_list;

void init_list(client_list *list);
void add_client(client_list *list, msgbuf message_rcv);
void remove_client(client_list *list, msgbuf message_rcv);
int get_clients_list(client_list *list, msgbuf *message_list);

#endif