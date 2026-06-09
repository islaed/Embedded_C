#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>
#include "driver_list.h"

// Список всех водителей
driver_list_t drivers;

// Доступные команды
const char *commands[] = {
    "help",
    "create_driver",
    "send_task",
    "get_status",
    "get_drivers",
    "exit"
};
// Что делает каждая команда
const char *describe[] = {
    "Вывод доступных команд",
    "Создать водителя",
    "Создать задачу водителю номер <pid> и занять его на <timer> секунд. send_task <pid> <task_timer>",
    "Показывает статус водителя с номером <pid>. get_status <pid>",
    "Показывает статусы и pid всех водителей",
    "Завершает работу программы"
};

// Показать список команд
void print_help()
{
    for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        printf("%s - %s\n", commands[i], describe[i]);
    }
}
// Создать нового водителя
void create_driver()
{
    int parent_to_child[2];
    int child_to_parent[2];

    // Делаем два пайпа для связи родителя и дочернего процесса
    if(pipe(parent_to_child) == -1)
    {
        perror("Pipe parent to child error!\n");
        return;
    }

    if(pipe(child_to_parent) == -1)
    {
        perror("Pipe child to parent error!\n");
        close(parent_to_child[0]);
        close(parent_to_child[1]);
        return;
    }

    pid_t pid = fork();

    if(pid == -1)
    {
        perror("Fork error!\n");
        close(parent_to_child[0]);
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        close(child_to_parent[1]);
        return;
    }

    if(pid == 0)
    {
        // Дочерний процесс
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        // Водитель ждёт команд
        while(1)
        {
            int timer;
            ssize_t bytes_read = read(parent_to_child[0], &timer, sizeof(int));

            if(bytes_read <= 0)
            {
                // Родитель закрыл пайп - выходим
                break;
            }

            // Выполнение задачи
            sleep(timer);

            // Говорим родителю что закончили
            int response = 0;
            write(child_to_parent[1], &response, sizeof(int));
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Родительский процесс
        close(parent_to_child[0]);
        close(child_to_parent[1]);

        // Добавляем водителя в список
        if(driver_list_add(&drivers, pid, parent_to_child[1], child_to_parent[0]) == 0)
        {
            printf("Создан водитель с PID: %d\n", pid);
        }
        else
        {
            printf("Ошибка добавления драйвера в список\n");
            kill(pid, SIGTERM);
            close(parent_to_child[1]);
            close(child_to_parent[0]);
        }
    }
}
// Дать задачу водителю
void send_task(char **arguments, int count)
{
    if(count < 3)
    {
        printf("Использование: send_task <pid> <task_timer>\n");
        return;
    }

    pid_t pid = atoi(arguments[1]);
    int timer = atoi(arguments[2]);

    if(timer <= 0)
    {
        printf("Таймер должен быть положительным числом\n");
        return;
    }

    int result = driver_send_task(&drivers, pid, timer);

    if(result == -1)
    {
        printf("Водитель с PID %d не найден\n", pid);
        return;
    }

    if(result > 0)
    {
        // Водитель занят
        printf("Busy %d\n", result);
        return;
    }

    // Водитель свободен, отправляем команду через пайп
    driver_node_t *driver = drivers.head;
    while(driver != NULL)
    {
        if(driver->pid == pid)
        {
            write(driver->fd_write, &timer, sizeof(int));
            printf("Задача отправлена водителю %d на %d секунд\n", pid, timer);
            return;
        }
        driver = driver->next;
    }
}
// Узнать статус водителя
void get_status(char **arguments, int count)
{
    if(count < 2)
    {
        printf("Использование: get_status <pid>\n");
        return;
    }

    pid_t pid = atoi(arguments[1]);
    driver_state_t state;
    int timer;

    if(driver_get_status(&drivers, pid, &state, &timer) == -1)
    {
        printf("Водитель с PID %d не найден\n", pid);
        return;
    }

    if(state == AVAILABLE)
    {
        printf("Available\n");
    }
    else
    {
        printf("Busy %d\n", timer);
    }
}
// Показать всех водителей
void get_drivers()
{
    driver_list_print(&drivers);
}
// Завершить все процессы водителей
void kill_drivers()
{
    driver_node_t *current = drivers.head;

    while(current != NULL)
    {
        printf("Завершение драйвера с PID: %d\n", current->pid);
        kill(current->pid, SIGTERM);
        waitpid(current->pid, NULL, 0);
        current = current->next;
    }

    // Чистим память после водителей
    current = drivers.head;
    driver_node_t *next;
    while(current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

int main()
{
    char input_buffer[256];
    char *arguments[3];

    driver_list_init(&drivers);

    printf("Введите \"help\" для получения списка доступных команд\n");
    printf("> ");
    fflush(stdout);

    while(1)
    {
        // Считаем количество водителей
        int driver_count = 0;
        driver_node_t *temp = drivers.head;
        while(temp != NULL)
        {
            driver_count++;
            temp = temp->next;
        }

        // Создаём массив для poll с stdin + все водители
        struct pollfd *fds = malloc(sizeof(struct pollfd) * (1 + driver_count));
        if(fds == NULL)
        {
            perror("Malloc error!\n");
            break;
        }

        // Первый это stdin
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        // Остальные - пайпы от водителей
        temp = drivers.head;
        int index = 1;
        while(temp != NULL)
        {
            fds[index].fd = temp->fd_read;
            fds[index].events = POLLIN;
            index++;
            temp = temp->next;
        }

        // Ждём событие с таймером в 1 секунду
        int poll_result = poll(fds, 1 + driver_count, 1000);

        if(poll_result == -1)
        {
            perror("poll");
            free(fds);
            break;
        }

        if(poll_result == 0)
        {
            // Прошла секунда, уменьшаем таймеры
            driver_list_update_timers(&drivers);
            free(fds);
            continue;
        }

        // Проверяем что пришло от пользователя
        if(fds[0].revents & POLLIN)
        {
            if(fgets(input_buffer, sizeof(input_buffer), stdin) == NULL)
            {
                free(fds);
                break;
            }

            int count = 0;
            // Разбиваем строку на части
            char *tok = strtok(input_buffer, " \t\n");
            while(tok != NULL && count < 3)
            {
                arguments[count++] = tok;
                tok = strtok(NULL, " \t\n");
            }

            // Ничего не ввели
            if(count == 0)
            {
                free(fds);
                printf("> ");
                fflush(stdout);
                continue;
            }

            // Смотрим какая команда
            if(strcmp(arguments[0], "help") == 0)
            {
                print_help();
            }
            else if(strcmp(arguments[0], "create_driver") == 0)
            {
                create_driver();
            }
            else if(strcmp(arguments[0], "send_task") == 0)
            {
                send_task(arguments, count);
            }
            else if(strcmp(arguments[0], "get_status") == 0)
            {
                get_status(arguments, count);
            }
            else if(strcmp(arguments[0], "get_drivers") == 0)
            {
                get_drivers();
            }
            else if(strcmp(arguments[0], "exit") == 0)
            {
                free(fds);
                kill_drivers();
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("Такой команды нет! Введите \"help\" для получения списка доступных команд\n");
            }

            // Выводим ">" для следующей команды
            printf("> ");
            fflush(stdout);
        }

        // Проверяем ответы от водителей
        temp = drivers.head;
        index = 1;
        while(temp != NULL)
        {
            if(fds[index].revents & POLLIN)
            {
                // Водитель закончил задачу
                int response;
                read(temp->fd_read, &response, sizeof(int));
            }
            index++;
            temp = temp->next;
        }

        free(fds);
    }
}