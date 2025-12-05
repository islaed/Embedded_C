// Напишите программу, которая ищет введенной строке (с клавиатуры)
// введенную подстроку (с клавиатуры) и возвращает указатель на начало 
// подстроки, если подстрока не найдена в указатель записывается NULL.
// В качестве срок использовать статические массивы.

#include <stdio.h>

#define N 20

int main()
{
    char str1[N];
    char str2[N];
    int counter_str1 = 0;
    int counter_str2 = 0;
    char* result = NULL;

    printf("Введите строку: ");
    scanf(" %[^\n]s", str1);
    printf("Введите подстроку: ");
    scanf(" %[^\n]s", str2);

    char* ptr;

    // Находим длину строки
    ptr = str1;
    while(*ptr)
    {
        // printf("%d ", *ptr);
        ptr++;
        counter_str1++;
    }

    // Находим длину подстроки
    ptr = str2;
    while(*ptr)
    {
        // printf("%d ", *ptr);
        ptr++;
        counter_str2++;
    }
    // printf("\nstr1: %d\nstr2: %d\n", counter_str1, counter_str2);

    if(counter_str1 < counter_str2)
    {
        printf("Количество символов в подстроке должно быть меньше, чем в основной строке!\n");
        return 0;
    }

    int flag; // Флаг для проверки совпадения
    for(int i = 0; i <= counter_str1 - counter_str2; i++)
    {
        flag = 1;

        for(int j = 0; j < counter_str2; j++)
        {
            if(str1[i + j] != str2[j])
            {
                flag = 0;
                break;
            }
        }

        if(flag)
        {
            result = &str1[i];
            break;
        }
    }

    if(result != NULL)
    {
        printf("Подстрока найдена\n");
        printf("Адрес: %p\n", result);
        printf("Позиция в строке: %ld\n", result - str1 + 1);
    }
    else
    {
        printf("Подстрока не найдена: %s\n", result);
    }
    
    return 0;
}