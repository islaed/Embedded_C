#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    // Если аргументов нет
    if(argc < 2)
    {
        printf("Использование: grep <строка> <файл> или через пайп \"| grep <строка>\"\n");
        return 1;
    }

    char *search_pattern = argv[1];
    FILE *input = NULL;

    // Если файл не указан, то проверяем, откуда идет ввод
    if(argc == 2)
    {
        if(isatty(fileno(stdin)))
        {
            printf("Ошибка: не указан файл для поиска или данные из пайпа.\n");
            return 1;
        }
        input = stdin;
    } 
    // Если файл указан, то открываем его
    else
    {
        char file_path[1024] = "";
        for(int i = 2; i < argc; i++)
        {
            strcat(file_path, argv[i]);
            if(i < argc - 1)
            {
                strcat(file_path, " ");
            }
        }

        input = fopen(file_path, "r");
        if(input == NULL)
        {
            printf("Ошибка: не удалось открыть файл '%s'\n", file_path);
            return 1;
        }
    }

    // Чтение и поиск
    char line[2048];
    while(fgets(line, sizeof(line), input) != NULL)
    {
        if(strstr(line, search_pattern) != NULL)
        {
            printf("%s", line);
        }
    }

    if(input != stdin)
    {
        fclose(input);
    }

    return 0;
}