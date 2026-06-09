#include "driver_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// Локальная функция, которая видна только в этом файле
static driver_node_t* find_driver(driver_list_t *list, pid_t pid)
{
    driver_node_t *current = list->head;
    while(current != NULL)
    {
        if(current->pid == pid)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Инициализация пустого списка
void driver_list_init(driver_list_t *list)
{
    list->head = NULL;
}

// Добавить водителя в список
int driver_list_add(driver_list_t *list, pid_t pid, int fd_write, int fd_read)
{
    driver_node_t *new_node = (driver_node_t*)malloc(sizeof(driver_node_t));
    if(new_node == NULL)
    {
        return -1;
    }

    new_node->pid = pid;
    new_node->state = AVAILABLE;
    new_node->timer = 0;
    new_node->fd_write = fd_write;
    new_node->fd_read = fd_read;
    new_node->next = list->head;
    list->head = new_node;

    return 0;
}

// Назначить задачу водителю
int driver_send_task(driver_list_t *list, pid_t pid, int timer)
{
    driver_node_t *driver = find_driver(list, pid);
    if(driver == NULL)
    {
        return -1; // Водитель не найден
    }

    if(driver->state == BUSY)
    {
        return driver->timer; // Возвращаем сколько ещё занят
    }

    driver->state = BUSY;
    driver->timer = timer;
    return 0;
}

// Узнать статус водителя
int driver_get_status(driver_list_t *list, pid_t pid, driver_state_t *state, int *timer)
{
    driver_node_t *driver = find_driver(list, pid);
    if(driver == NULL)
    {
        return -1;
    }

    *state = driver->state;
    *timer = driver->timer;
    return 0;
}

// Показать всех водителей
void driver_list_print(driver_list_t *list)
{
    driver_node_t *current = list->head;

    if(current == NULL)
    {
        printf("Нет активных водителей\n");
        return;
    }

    while(current != NULL)
    {
        printf("PID: %d, Состояние: ", current->pid);
        if(current->state == AVAILABLE)
        {
            printf("Available\n");
        }
        else
        {
            printf("Busy %d\n", current->timer);
        }
        current = current->next;
    }
}

// Обновление таймеров водителей
void driver_list_update_timers(driver_list_t *list)
{
    driver_node_t *current = list->head;
    while(current != NULL)
    {
        if(current->state == BUSY && current->timer > 0)
        {
            current->timer--;
            if(current->timer == 0)
            {
                current->state = AVAILABLE; // Время вышло, водитель свободен
            }
        }
        current = current->next;
    }
}