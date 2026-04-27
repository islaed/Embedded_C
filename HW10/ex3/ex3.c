#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>


const char *commands[] = {
    "help",
    "clear",
    "exit",
    "fm",
    "sd",
    "calc",
    "pwd",
    "cd",
    "ls"
};
const char *describe[] = {
    "Выводит доступные команды",
    "Очистить экран",
    "Выход",
    "Файловый менеджер",
    "Абонентский справочник",
    "Калькулятор. Использует формат: calc <число> <операция> <число>",
    "Выводит текущую рабочую директорию",
    "Переход в директорию. Использует формат: cd <путь>",
    "Выводит файлы в директории. Использует формат: ls <путь>"
};

void change_dir(char *arguments[], int arguments_counter)
{
    if(arguments[1] == NULL)
    {
        if(chdir("/home/") != 0)
        {
            printf("Ошибка при переходе в директорию!\n");
        }
    }
    else
    {
        char full_path[1024] = "";
        // Склеиваем аргументы для папок, у которых в названии есть пробел
        for(int i = 1; i < arguments_counter; i++)
        {
            strcat(full_path, arguments[i]);
            if(i < arguments_counter - 1)
            {
                strcat(full_path, " ");
            }
        }

        if(chdir(full_path) != 0)
        {
            printf("Ошибка при переходе в директорию!\n");
        }
    }
}

void print_help()
{
    for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        printf("%s - %s\n", commands[i], describe[i]);
    }
}

void run(char *arguments[], char *files_path)
{
    char full_path[1024];
    // Склеиваем строки, чтобы получить полный путь до папки с бинарными файлами
    snprintf(full_path, sizeof(full_path), "%s/files/%s", files_path, arguments[0]);

    // Создаём процесс, в котором запускаем заданную программу
    pid_t pid = fork();
    if (pid == 0)
    {
        execv(full_path, arguments);
        printf("Ошибка при запуске файла!");
        exit(1);
    }
    wait(NULL);
}

int main()
{
    char user_request[1024];
    char *arguments[16];

    char files_path[256];
    getcwd(files_path, sizeof(files_path));

    printf("\nЧтобы увидеть все доступные команды введите \"help\"\n");

    while(1)
    {
        char cwd[256];
        getcwd(cwd, sizeof(cwd));

        // \033[1;34m для синего цвета
        // \033[1;32m для зелёного цвета
        // \033[0m для того чтобы сбросить настройки цвета
        printf("\033[1;34m%s\033[1;32m>\033[0m ", cwd);
        fgets(user_request, sizeof(user_request), stdin);

        int arguments_counter = 0;
        char *tok = strtok(user_request, " \t\n"); // Вычленяем первое слово
        while (tok != NULL && arguments_counter < 15)
        {
            arguments[arguments_counter] = tok; // Записываем в массив первое слово
            arguments_counter++;
            tok = strtok(NULL, " \t\n"); // Вычленяем следующее слово
        }
        arguments[arguments_counter] = NULL;

        // Если строка пустая
        if(arguments_counter == 0)
            continue;

        // Проверяем есть ли команда в списке
        int in_list = 0;
        for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
        {
            if(strcmp(arguments[0], commands[i]) == 0)
            {
                in_list = 1;
                break;
            }
        }
        if(!in_list)
        {
            printf("%s - Такой команды нет! Чтобы узнать доступные команды введите \"help\"\n", arguments[0]);
            continue;
        }

        if(strcmp(arguments[0], "help") == 0)
            print_help();
        else if(strcmp(arguments[0], "clear") == 0)
            printf("\033[H\033[J");
        else if(strcmp(arguments[0], "exit") == 0)
            exit(0);
        else if(strcmp(arguments[0], "cd") == 0)
            change_dir(arguments, arguments_counter);
        else
            run(arguments, files_path);
    }

    return 0;
}