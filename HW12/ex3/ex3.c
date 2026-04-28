#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>


const char *commands[] = {
    "help",
    "clear",
    "exit",
    "fm",
    "sd",
    "calc",
    "pwd",
    "cd",
    "ls",
    "cat",
    "grep",
    "wc"
};
const char *describe[] = {
    "Выводит доступные команды",
    "Очистить экран",
    "Выход",
    "Файловый менеджер",
    "Абонентский справочник",
    "Калькулятор. Использование: calc <число> <операция> <число>",
    "Выводит текущую рабочую директорию",
    "Переход в директорию. Использование: cd <путь>",
    "Выводит файлы в директории. Использование: ls <путь> (принимает параметры -l, -a)",
    "Читает содержимое файла. Использование: cat <файл>",
    "Поиск по строке. Использование: grep <строка> <файл> или через пайп",
    "Считает количество строк. Использование: wc <файл> или через пайп"
};

// Проверка существования команды в списке
int command_exists(char *cmd)
{
    for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        if(strcmp(cmd, commands[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Запуск бинарника из папки files
void exec_file(char **args, char *files_path)
{
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/files/%s", files_path, args[0]);
    
    execv(full_path, args);

    // Если execv не сработал, значит файла нет или ошибка
    printf("Ошибка запуска файла \"%s\"!\n", args[0]);
    exit(1);
}
// Смена директории
void change_dir(char *arguments[], int arguments_counter)
{
    if(arguments[1] == NULL)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        char full_path[1024] = "";
        for(int i = 1; i < arguments_counter; i++)
        {
            strcat(full_path, arguments[i]);
            if(i < arguments_counter - 1) strcat(full_path, " ");
        }
        if(chdir(full_path) != 0)
        {
            printf("Ошибка при переходе в директорию!\n");
        }
    }
}
// Вывод доступных команд
void print_help()
{
    for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        printf("%s - %s\n", commands[i], describe[i]);
    }
}
// 
void run(char *args[], int count, char *files_path)
{
    int cmd_count = 0;
    char *cmds[16][16];
    int cmd_i = 0, arg_i = 0;

    for(int i = 0; i < count; i++)
    {
        if(strcmp(args[i], "|") == 0)
        {
            cmds[cmd_i][arg_i] = NULL;
            cmd_i++;
            arg_i = 0;
        }
        else
        {
            cmds[cmd_i][arg_i++] = args[i];
            cmds[cmd_i][arg_i] = NULL;
        }
    }
    cmd_count = cmd_i + 1;

    // Проверка первой команды
    if(!command_exists(cmds[0][0]))
    {
        printf("%s: Команда не найдена! Чтобы увидеть все доступные команды введите \"help\"\n", cmds[0][0]);
        return;
    }

    // Если без пайпов, то просто запускаем бинарник с аргументами
    if(cmd_count == 1)
    {
        pid_t proc = fork();
        if(proc == 0)
        {
            exec_file(cmds[0], files_path);
        }
        wait(NULL);
        return;
    }

    // Создаём дескриптор для пайпов
    int pipefds[2 * (cmd_count - 1)];
    for(int i = 0; i < cmd_count - 1; i++)
    {
        if(pipe(pipefds + i * 2) < 0)
        {
            return;
        }
    }

    for(int i = 0; i < cmd_count; i++)
    {
        pid_t proc = fork();
        if(proc == 0)
        {
            // Если команда не первая, то забирает данные из пайпа
            if(i > 0)
            {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }
            // Если команда не последняя, то она передаёт данные в пайп
            if(i < cmd_count - 1)
            {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }
            // Закрываем пайпы
            for(int j = 0; j < 2 * (cmd_count - 1); j++)
            {
                close(pipefds[j]);
            }

            exec_file(cmds[i], files_path);
        }
    }

    for(int i = 0; i < 2 * (cmd_count - 1); i++)
    {
        close(pipefds[i]);
    }
    for(int i = 0; i < cmd_count; i++)
    {
        wait(NULL);
    }
}

int main()
{
    char user_request[1024];
    char *arguments[64];
    char files_path[256];
    getcwd(files_path, sizeof(files_path));

    printf("\nЧтобы увидеть все доступные команды введите \"help\"\n");

    while(1)
    {
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        // \033[1;34m пишем жирным синим цветом
        // \033[1;32m пишем жирным зелёным цветом
        // \033[0m сбрасываем все настройки
        printf("\033[1;34m%s\033[1;32m>\033[0m ", cwd);

        if(!fgets(user_request, sizeof(user_request), stdin))
        {
            break;
        }

        int count = 0;
        char *tok = strtok(user_request, " \t\n");
        while(tok != NULL && count < 63)
        {
            arguments[count++] = tok;
            tok = strtok(NULL, " \t\n");
        }
        arguments[count] = NULL;

        if(count == 0)
        {
            continue;
        }

        if(strcmp(arguments[0], "help") == 0)
        {
            print_help();
        }
        else if(strcmp(arguments[0], "clear") == 0)
        {
            // \033[2J очистить экран
            // \033[3J очистить буфер прокрутки
            // \033[H возвращение курсора в левый верхний угол
            printf("\033[2J\033[3J\033[H");
        }
        else if(strcmp(arguments[0], "exit") == 0)
        {
            exit(0);
        }
        else if(strcmp(arguments[0], "cd") == 0)
        {
            change_dir(arguments, count);
        }
        else
        {
            run(arguments, count, files_path);
        }
    }
    return 0;
}