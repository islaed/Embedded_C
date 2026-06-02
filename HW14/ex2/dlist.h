#ifndef DLIST_H
#define DLIST_H

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NAME_SIZE 32 // Максимальный размер имени
#define MAX_TEXT_SIZE 256 // Максимальный размер текста в сообщении
#define MAX_CLIENTS 32
#define MAX_MESSAGES 10 // Максимальное количество сообщений в буфере

// Типы сообщений
#define MSG_AUTH 1      // Авторизация клиента
#define MSG_LIST 2      // Список клиентов
#define MSG_TEXT 3      // Текстовое сообщение от клиента
#define MSG_BROADCAST 4 // Широковещательное сообщение
#define MSG_DEAUTH 5    // Деавторизация клиента

// Структура для отправки сообщений
typedef struct msgbuf
{
    int mtype;

    pid_t client_pid; // pid клиента
    int client_shm_id; // ID разделяемой памяти клиента

    char name[MAX_NAME_SIZE]; // Имя клиента
    char text[MAX_TEXT_SIZE]; // Текст сообщения клиента
    char auth_clients[MAX_CLIENTS][MAX_NAME_SIZE]; // Список клиентов
    int clients_count; // Количество подключенных клиентов
} msgbuf;


// Кольцевой буфер для сообщений в разделяемой памяти
typedef struct message_buffer
{
    msgbuf messages[MAX_MESSAGES];
    int read_pos;
    int write_pos;
    int count;
} message_buffer;

// Структура клиента для двусвязного списка
typedef struct client_node
{
    struct client_node *next;
    struct client_node *prev;

    pid_t client_pid; // pid клиента
    int client_shm_id; // ID разделяемой памяти клиента

    char name[MAX_NAME_SIZE]; // Имя клиента
} client_node;

// Указатели на первый и последний элемент в двусвязном списке
typedef struct client_list
{
    client_node *head;
    client_node *tail;
} client_list;

// Операции с семафорами
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void sem_wait_custom(int semid, int sem_num);
void sem_post_custom(int semid, int sem_num);

void init_list(client_list *list);
void add_client(client_list *list, msgbuf message_rcv);
void remove_client(client_list *list, msgbuf message_rcv);
int get_clients_list(client_list *list, msgbuf *message_list);

#endif