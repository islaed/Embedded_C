#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    char full_path[1024] = "";
    // Склеиваем аргументы, если есть пробелы в имени файла
    for (int i = 1; i < argc; i++)
    {
        strcat(full_path, argv[i]);
        if(i < argc - 1)
        {
            strcat(full_path, " ");
        }
    }

    // Если путь пустой
    if(strlen(full_path) == 0)
    {
        printf("Использование: cat <файл>\n");
        return 0;
    }

    FILE *file = fopen(full_path, "r");
    if(file == NULL)
    {
        printf("Ошибка: файл \"%s\" не найден!\n", full_path);
        return 1;
    }

    char buffer[1024];
    while(fgets(buffer, sizeof(buffer), file) != NULL)
    {
        printf("%s", buffer);
    }

    fclose(file);
    printf("\n");
    return 0;
}