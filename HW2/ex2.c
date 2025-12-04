// Вывести заданный массив размером N в обратном порядке.

#include <stdio.h>

#define N 7

int main()
{
    int a[N];
    int b[N];

    for(int i = 0; i < N; i++)
    {
        a[i] = i + 1;
        printf("%d ", a[i]);
    }
    printf("\n");

    for(int i = 0; i < N; i++)
    {
        b[i] = a[N - 1 - i];
        printf("%d ", b[i]);
    }
    printf("\n");

    return 0;
}