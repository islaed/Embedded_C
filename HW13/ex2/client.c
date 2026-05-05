#include <ncursesw/ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include "dlist.h"

#define Q_MAIN "."

int main_qid;
int client_qid;
char name[MAX_NAME_SIZE];
pid_t pid;

WINDOW *chat_win;
WINDOW *chat_content; // Окно для текста сообщений
WINDOW *users_win;
WINDOW *input_win;
pthread_mutex_t ncurses_mutex;

// Обработчик сигналов для перехвата ctrl+c
void handle_signal(int sig)
{
    msgbuf message_snd;
    message_snd.mtype = 5;
    message_snd.client_pid = pid;
    strcpy(message_snd.name, name);

    // Отправляем сигнал на сервер перед выходом
    msgsnd(main_qid, &message_snd, sizeof(message_snd) - sizeof(long), IPC_NOWAIT);

    pthread_mutex_destroy(&ncurses_mutex);
    endwin();
    exit(0);
}

// Поток отправляющий сообщения
void *message_send(void *args)
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

        // Проверяем, ввел ли клиент хоть что-то
        if (strlen(message) == 0)
        {
            continue;
        }

        if(strcmp(message, "exit") == 0)
        {
            message_snd.mtype = 5;
            message_snd.client_pid = pid;
        }
        else
        {
            message_snd.mtype = 3;
            strcpy(message_snd.text, message);
        }
        strcpy(message_snd.name, name);

        int msg_snd = msgsnd(main_qid, &message_snd, sizeof(message_snd) - sizeof(long), IPC_NOWAIT);
        if(msg_snd == -1)
        {
            continue;
        }
        if(message_snd.mtype == 5)
        {
            pthread_mutex_destroy(&ncurses_mutex);
            endwin();
            exit(0);
        }
    }
}

// Поток принимающий сообщения
void *message_recieve(void *args)
{
    msgbuf message_rcv;
    
    while(1)
    {
        ssize_t message_read = msgrcv(client_qid, &message_rcv, sizeof(msgbuf) - sizeof(long), 0, 0);
        if(message_read == -1)
        {
            continue;
        }
        switch(message_rcv.mtype)
        {
            case 2: // Получаем список клиентов на сервере
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
                
                // Перерисовываем границы
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
            
            case 4: // Получаем сообщение от клиента
                pthread_mutex_lock(&ncurses_mutex);
                // Выводим текст в дочернее окно, не затирая рамку
                wprintw(chat_content, "[%s]: %s\n", message_rcv.name, message_rcv.text);
                
                // Перерисовываем границы, чтобы они не затирались текстом
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
    setlocale(LC_ALL, ""); // Чтобы были видны русские буквы

    // Регистрируем перехват сигналов
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    pid = getpid();
    key_t key_client;
    key_client = ftok(Q_MAIN, pid);
    
    printf("Введите своё имя: ");
    fgets(name, MAX_NAME_SIZE, stdin);
    // Удаляем символ "\n"
    name[strcspn(name, "\n")] = 0;
    
    msgbuf client_data;
    client_data.mtype = 1;
    client_data.client_key = key_client;
    client_data.client_pid = pid;
    strcpy(client_data.name, name);
    
    // Создаём личную очередь для клиента
    client_qid = msgget(key_client, IPC_CREAT | 0666);
    if(client_qid == -1)
    {
        printf("Ошибка открытия личной очереди!\n");
        return 1;
    }

    // Подключаемся к очереди сервера
    key_t key_main;
    key_main = ftok(Q_MAIN, 0);
    main_qid = msgget(key_main, 0);
    if(main_qid == -1)
    {
        printf("Ошибка открытия общей очереди!\n");
        return 1;
    }

    // Отправляем свои данные на сервер
    int msg_snd = msgsnd(main_qid, &client_data, sizeof(client_data) - sizeof(long), IPC_NOWAIT);
    if(msg_snd == -1)
    {
        printf("Ошибка отправки данных!\n");
        return 1;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Размеры окон
    int chat_width = COLS - 25;
    int chat_height = LINES - 3;
    int users_width = 25;
    int users_height = LINES - 3;
    int input_height = 3;
    int input_width = COLS;

    // Инициализация окон
    chat_win  = newwin(chat_height, chat_width, 0, 0);
    
    // Создаем внутреннее окно для сообщений
    chat_content = derwin(chat_win, chat_height - 2, chat_width - 4, 1, 2);
    users_win = newwin(users_height, users_width, 0, chat_width);
    input_win = newwin(input_height, input_width, LINES - 3, 0);

    scrollok(chat_content, TRUE);

    // Отрисовка рамок
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

    pthread_t recieve;
    pthread_t send;
    
    pthread_create(&recieve, NULL, &message_recieve, NULL);
    pthread_create(&send, NULL, &message_send, NULL);

    pthread_join(recieve, NULL);
    pthread_join(send, NULL);
    
    pthread_mutex_destroy(&ncurses_mutex);
    endwin();
    return 0;
}