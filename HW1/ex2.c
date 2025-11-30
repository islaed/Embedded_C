// Вывести двоичное представление целого отрицательного числа, 
// используя битовые операции (число вводится с клавиатуры).

#include <stdio.h>

int main() 
{
    int number;

    printf("Введите целое отрицательное число: ");
    scanf("%d", &number);

    if (number < 0)
    {
        int bit = sizeof(number) * 8;
        
        for (int i = bit - 1; i >= 0; i--) 
        {
            if ((number >> i) & 1)
            {
                printf("1");
            }
            else
            {
                printf("0");
            }
        }
        printf("\n");
    }
    else
    {
        printf("Введите отрицательное число!\n");
    }
    return 0;
}