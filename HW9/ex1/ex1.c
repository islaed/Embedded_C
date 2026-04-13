#include <stdio.h>

int main()
{
    FILE *file;
    file = fopen("output.txt", "w+"); // Создание файла
    if(file == NULL)
    {
        printf("Ошибка открытия файла!");
        return 0;
    }

    fputs("String from file", file); // Запись строки в файл

    fseek(file, 0, SEEK_END); // Перестановка курсора в конец
    int pos = ftell(file); // Позиция курсора в файле

    while(pos > 0)
    {
        pos--; // Сначала отнимаем единицу, чтобы не попасть на служебный символ в конце файла
        fseek(file, pos, SEEK_SET); // Переставляем курсор от начала файла со сдвигом pos
        printf("%c", fgetc(file));
    }
    printf("\n");

    fclose(file);
    return 0;
}
