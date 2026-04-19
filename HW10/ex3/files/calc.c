#include <stdio.h>
#include <stdlib.h>


int main(int arguments_counter, char *arguments[])
{
    if(arguments_counter != 4)
    {
        printf("Ошибка: Неправльный ввод аргументов!\n");
        return 1;
    }

    float num1 = atof(arguments[1]); // Конвертируем строку в число
    char operation = arguments[2][0]; // Берем первый символ из строки операции
    float num2 = atof(arguments[3]);
    float result;

    switch(operation)
    {
        case '+':
            result = num1 + num2;
            break;

        case '-':
            result = num1 - num2;
            break;

        case '*':
            result = num1 * num2;
            break;

        case '/':
            if (num2 == 0) {
                printf("Ошибка: Деление на ноль!\n");
                return 1;
            }
            result = num1 / num2;
            break;

        default:
            printf("Ошибка: Неизвестная операция \"%c\"\n", operation);
            return 1;
    }

    printf("%.2f %c %.2f = %.2f\n", num1, operation, num2, result);

    return 0;
}