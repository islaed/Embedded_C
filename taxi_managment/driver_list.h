#ifndef DRIVER_LIST_H
#define DRIVER_LIST_H

#include <sys/types.h>

// Состояния водителя
typedef enum {
    AVAILABLE,
    BUSY
} driver_state_t;

// Информация о водителе
typedef struct driver_node {
    pid_t pid; // pid водителя
    driver_state_t state; // Текущее состояние водителя
    int timer; // Оставшееся время задачи
    int fd_write; // Пайп для отправки команд водителю
    int fd_read; // Пайп для получения ответов от водителя
    struct driver_node *next;
} driver_node_t;

// Список водителей
typedef struct {
    driver_node_t *head;
} driver_list_t;

// Инициализация пустого списка
void driver_list_init(driver_list_t *list);

// Добавить водителя в список
int driver_list_add(driver_list_t *list, pid_t pid, int fd_write, int fd_read);

// Назначить задачу водителю
int driver_send_task(driver_list_t *list, pid_t pid, int timer);

// Узнать статус водителя
int driver_get_status(driver_list_t *list, pid_t pid, driver_state_t *state, int *timer);

// Показать всех водителей
void driver_list_print(driver_list_t *list);

// Обновление таймеров водителей
void driver_list_update_timers(driver_list_t *list);

#endif
