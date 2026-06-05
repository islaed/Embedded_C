#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Инициализация очереди
int queue_init(queue_t *queue)
{
    queue->head = NULL;
    queue->tail = NULL;

    if(pthread_mutex_init(&queue->lock, NULL) != 0)
    {
        return -1;
    }

    if(pthread_cond_init(&queue->cond, NULL) != 0)
    {
        pthread_mutex_destroy(&queue->lock);
        return -1;
    }

    return 0;
}

// Добавление адреса клиента в очередь
void queue_add(queue_t *queue, client_addr_t *client)
{
    node_t *new_node = malloc(sizeof(node_t));
    if(!new_node)
    {
        perror("malloc");
        return;
    }

    // Копируем адрес клиента
    memcpy(&new_node->client, client, sizeof(client_addr_t));
    new_node->next = NULL;

    pthread_mutex_lock(&queue->lock);

    if(queue->tail == NULL)
    {
        queue->head = new_node;
        queue->tail = new_node;
    }
    else
    {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }

    // Будим один ожидающий поток
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

// Забираем адрес первого клиента из очереди (блокируется если пусто)
void queue_get(queue_t *queue, client_addr_t *client)
{
    pthread_mutex_lock(&queue->lock);

    // Ждём пока очередь не станет непустой
    while(queue->head == NULL)
    {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }

    node_t *node = queue->head;
    memcpy(client, &node->client, sizeof(client_addr_t));

    queue->head = node->next;
    if(queue->head == NULL)
    {
        queue->tail = NULL;
    }

    pthread_mutex_unlock(&queue->lock);

    free(node);
}

// Удаляем очередь
void queue_destroy(queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);

    node_t *current = queue->head;
    while(current != NULL)
    {
        node_t *next = current->next;
        free(current);
        current = next;
    }

    queue->head = NULL;
    queue->tail = NULL;

    pthread_mutex_unlock(&queue->lock);

    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->lock);
}
