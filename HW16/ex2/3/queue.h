#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <netinet/in.h>

// Структура для хранения адреса клиента
typedef struct {
    struct sockaddr_in addr;
    socklen_t addr_len;
} client_addr_t;

typedef struct node {
    client_addr_t client;
    struct node *next;
} node_t;

typedef struct {
    node_t *head;
    node_t *tail;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} queue_t;

int queue_init(queue_t *queue);
void queue_add(queue_t *queue, client_addr_t *client);
void queue_get(queue_t *queue, client_addr_t *client);
void queue_destroy(queue_t *queue);

#endif
