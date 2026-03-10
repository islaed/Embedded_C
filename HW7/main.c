#include <stdio.h>

int add(int first_num, int second_num);
int sub(int first_num, int second_num);
int mul(int first_num, int second_num);
float div(int first_num, int second_num);

int main()
{
    char number;
    int first_num, second_num;

    while(1)
    {
        printf("\nВведите первое число: ");
        scanf("%d", &first_num);
        printf("Введите второе число: ");
        scanf("%d", &second_num);

        printf("\nВыберите действие");
        printf("\n1)Сложение\n2)Вычитание\n3)Умножение\n4)Деление\n5)Выход\n\nВыберите пункт: ");
        scanf(" %c", &number);
        printf("\n");

        switch(number)
        {
            case '1':
                printf("Результат сложения: %d\n", add(first_num, second_num));
                break;
            case '2':
                printf("Результат вычитания: %d\n", sub(first_num, second_num));
                break;
            case '3':
                printf("Результат умножения: %d\n", mul(first_num, second_num));
                break;
            case '4':
                printf("Результат деления: %.1f\n", div(first_num, second_num));
                break;
            case '5':
                printf("Завершение программы.\n");
                return 0;
            default:
                printf("Такого пункта нет!");
                break;
        }
    }
}