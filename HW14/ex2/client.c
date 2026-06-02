#include <ncursesw/ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include "dlist.h"

#define Q_MAIN "."

int server_shm_id;
int server_sem_id;
int client_shm_id;
int client_sem_id;
message_buffer *server_buf;
message_buffer *client_buf;
char name[MAX_NAME_SIZE];
pid_t pid;

WINDOW *chat_win;
WINDOW *chat_content;
WINDOW *users_win;
WINDOW *input_win;
pthread_mutex_t ncurses_mutex;

// Отправка сообщения на сервер
void send_to_server(msgbuf *message)
{
    // Ждем освобождения места в буфере сервера
    sem_wait_custom(server_sem_id, 1); // Семафор пустых слотов

    // Записываем сообщение
    server_buf->messages[server_buf->write_pos] = *message;
    server_buf->write_pos = (server_buf->write_pos + 1) % MAX_MESSAGES;
    server_buf->count++;

    // Сигнализируем о новом сообщении
    sem_post_custom(server_sem_id, 0); // Семафор заполненных слотов
}

// Обработчик сигналов для перехвата ctrl+c
void handle_signal(int sig)
{
    msgbuf message_snd;
    message_snd.mtype = MSG_DEAUTH;
    message_snd.client_pid = pid;
    strcpy(message_snd.name, name);

    send_to_server(&message_snd);

    pthread_mutex_destroy(&ncurses_mutex);

    shmdt(server_buf);
    shmdt(client_buf);
    shmctl(client_shm_id, IPC_RMID, NULL);
    semctl(client_sem_id, 0, IPC_RMID);

    endwin();
    exit(0);
}

// Поток отправляющий сообщения
void *message_send_thread(void *args)
{
    char message[MAX_TEXT_SIZE];
    msgbuf message_snd;

    while(1)
    {
        pthread_mutex_lock(&ncurses_mutex);
        wclear(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 2, "Сообщение: ");
        wrefresh(input_win);
        pthread_mutex_unlock(&ncurses_mutex);

        echo();
        wmove(input_win, 1, 13);
        wgetnstr(input_win, message, MAX_TEXT_SIZE);
        noecho();

        pthread_mutex_lock(&ncurses_mutex);
        wclear(input_win);
        box(input_win, 0, 0);
        wrefresh(input_win);
        pthread_mutex_unlock(&ncurses_mutex);

        if (strlen(message) == 0)
        {
            continue;
        }

        if(strcmp(message, "exit") == 0)
        {
            message_snd.mtype = MSG_DEAUTH;
            message_snd.client_pid = pid;
        }
        else
        {
            message_snd.mtype = MSG_TEXT;
            strcpy(message_snd.text, message);
        }
        strcpy(message_snd.name, name);

        send_to_server(&message_snd);

        if(message_snd.mtype == MSG_DEAUTH)
        {
            pthread_mutex_destroy(&ncurses_mutex);
            shmdt(server_buf);
            shmdt(client_buf);
            shmctl(client_shm_id, IPC_RMID, NULL);
            semctl(client_sem_id, 0, IPC_RMID);
            endwin();
            exit(0);
        }
    }
}

// Поток принимающий сообщения
void *message_receive_thread(void *args)
{
    while(1)
    {
        // Ждем сообщения
        sem_wait_custom(client_sem_id, 0); // Семафор заполненных слотов

        // Читаем сообщение
        msgbuf message_rcv = client_buf->messages[client_buf->read_pos];
        client_buf->read_pos = (client_buf->read_pos + 1) % MAX_MESSAGES;
        client_buf->count--;

        // Освобождаем слот
        sem_post_custom(client_sem_id, 1); // Семафор пустых слотов

        switch(message_rcv.mtype)
        {
            case MSG_LIST:
                pthread_mutex_lock(&ncurses_mutex);
                wclear(users_win);
                box(users_win, 0, 0);
                wattron(users_win, A_BOLD);
                mvwprintw(users_win, 0, 2, "Пользователи");
                wattroff(users_win, A_BOLD);

                for(int i = 0; i < message_rcv.clients_count; i++)
                {
                    mvwprintw(users_win, 1 + i, 2, "%s", message_rcv.auth_clients[i]);
                }

                box(chat_win, 0, 0);
                wattron(chat_win, A_BOLD);
                mvwprintw(chat_win, 0, 2, "Сообщения");
                wattroff(chat_win, A_BOLD);

                wrefresh(users_win);
                wrefresh(chat_win);
                wrefresh(chat_content);
                wrefresh(input_win);
                pthread_mutex_unlock(&ncurses_mutex);
                break;

            case MSG_BROADCAST:
                pthread_mutex_lock(&ncurses_mutex);
                wprintw(chat_content, "[%s]: %s\n", message_rcv.name, message_rcv.text);

                box(chat_win, 0, 0);
                wattron(chat_win, A_BOLD);
                mvwprintw(chat_win, 0, 2, "Сообщения");
                wattroff(chat_win, A_BOLD);

                wrefresh(chat_win);
                wrefresh(chat_content);
                wrefresh(input_win);
                pthread_mutex_unlock(&ncurses_mutex);
                break;
        }
    }
}

int main()
{
    setlocale(LC_ALL, "");

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    pid = getpid();

    printf("Введите своё имя: ");
    fgets(name, MAX_NAME_SIZE, stdin);
    name[strcspn(name, "\n")] = 0;

    // Подключаемся к серверу
    key_t key_main = ftok(Q_MAIN, 0);
    server_shm_id = shmget(key_main, sizeof(message_buffer), 0);
    if(server_shm_id == -1)
    {
        printf("Ошибка открытия памяти сервера!\n");
        return 1;
    }

    server_buf = (message_buffer *)shmat(server_shm_id, NULL, 0);
    if(server_buf == (void *)-1)
    {
        printf("Ошибка подключения к памяти сервера!\n");
        return 1;
    }

    server_sem_id = semget(key_main, 2, 0);
    if(server_sem_id == -1)
    {
        printf("Ошибка открытия семафоров сервера!\n");
        return 1;
    }

    // Создаём свою разделяемую память
    key_t key_client = ftok(Q_MAIN, pid);
    client_shm_id = shmget(key_client, sizeof(message_buffer), IPC_CREAT | 0666);
    if(client_shm_id == -1)
    {
        printf("Ошибка создания памяти клиента!\n");
        return 1;
    }

    client_buf = (message_buffer *)shmat(client_shm_id, NULL, 0);
    if(client_buf == (void *)-1)
    {
        printf("Ошибка подключения к памяти клиента!\n");
        return 1;
    }

    // Инициализируем буфер
    client_buf->read_pos = 0;
    client_buf->write_pos = 0;
    client_buf->count = 0;

    // Создаём семафоры
    client_sem_id = semget(key_client, 2, IPC_CREAT | 0666);
    if(client_sem_id == -1)
    {
        printf("Ошибка создания семафоров!\n");
        return 1;
    }

    union semun arg;
    arg.val = 0;
    semctl(client_sem_id, 0, SETVAL, arg); // Заполненные слоты = 0
    arg.val = MAX_MESSAGES;
    semctl(client_sem_id, 1, SETVAL, arg); // Пустые слоты = MAX_MESSAGES

    // Отправляем данные на сервер для авторизации
    msgbuf client_data;
    client_data.mtype = MSG_AUTH;
    client_data.client_shm_id = client_shm_id;
    client_data.client_pid = pid;
    strcpy(client_data.name, name);

    send_to_server(&client_data);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int chat_width = COLS - 25;
    int chat_height = LINES - 3;
    int users_width = 25;
    int users_height = LINES - 3;
    int input_height = 3;
    int input_width = COLS;

    chat_win = newwin(chat_height, chat_width, 0, 0);
    chat_content = derwin(chat_win, chat_height - 2, chat_width - 4, 1, 2);
    users_win = newwin(users_height, users_width, 0, chat_width);
    input_win = newwin(input_height, input_width, LINES - 3, 0);

    scrollok(chat_content, TRUE);

    box(chat_win, 0, 0);
    box(users_win, 0, 0);
    box(input_win, 0, 0);

    wattron(chat_win, A_BOLD);
    mvwprintw(chat_win, 0, 2, "Сообщения");
    wattroff(chat_win, A_BOLD);

    wattron(users_win, A_BOLD);
    mvwprintw(users_win, 0, 2, "Пользователи");
    wattroff(users_win, A_BOLD);

    wrefresh(chat_win);
    wrefresh(chat_content);
    wrefresh(users_win);
    wrefresh(input_win);

    pthread_mutex_init(&ncurses_mutex, NULL);

    pthread_t receive;
    pthread_t send;

    pthread_create(&receive, NULL, &message_receive_thread, NULL);
    pthread_create(&send, NULL, &message_send_thread, NULL);

    pthread_join(receive, NULL);
    pthread_join(send, NULL);

    pthread_mutex_destroy(&ncurses_mutex);
    endwin();
    return 0;
}
