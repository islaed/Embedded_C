#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    FILE *input = NULL;
    int line_count = 0;
    char buffer[4096];

    // Если аргумент есть
    if(argc > 1)
    {
        input = fopen(argv[1], "r");
        if(input == NULL)
        {
            printf("Ошибка открытия файла!");
            return 1;
        }
    } 
    
    // Если аргументов нет, проверяем пайп
    else
    {
        // Если нет данных из пайпа, то выдаем ошибку
        if(isatty(fileno(stdin)))
        {
            printf("Использование: wc <файл> или через пайп \"wc\"\n");
            return 1;
        }
        input = stdin;
    }

    // Считаем строки
    while(fgets(buffer, sizeof(buffer), input) != NULL)
    {
        line_count++;
    }

    printf("%d\n", line_count);

    if(input != stdin)
    {
        fclose(input);
    }

    return 0;
}