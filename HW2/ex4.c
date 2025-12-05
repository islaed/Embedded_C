// Заполнить матрицу числами от 1 до N^2 улиткой

#include <stdio.h>

#define N 5

int main()
{
    int a[N][N];

    // Объявляем счётчик
    int number = 1;
    
    // Объявляем границы
    int top = 0, left = 0;
    int bottom = N - 1, right = N - 1;

    while(number <= N*N)
    {
        // Заполняем верхнюю строку слева направо
        for(int i = left; i <= right; i++) 
        {
            a[top][i] = number;
            number++;
        }
        top++;

        // Заполняем правый столбец сверху вниз
        for(int i = top; i <= bottom; i++) 
        {
            a[i][right] = number;
            number++;
        }
        right--;

        // Заполняем нижнюю строку справа налево
        for(int i = right; i >= left; i--) 
        {
            a[bottom][i] = number;
            number++;
        }
        bottom--;

        // Заполняем левый столбец снизу вверх
        for(int i = bottom; i >= top; i--) 
        {
            a[i][left] = number;
            number++;
        }
        left++;
    }

    // Выводим массив
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            printf("%3d ", a[i][j]);
        }
        printf("\n");
    }

    return 0;
}