// Вывести квадратную матрицу по заданному N.

#include <stdio.h>

#define N 5

int main()
{
    int a[N][N];

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            a[i][j] = j + 1 + (i * N);
            printf("%3d ", a[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}