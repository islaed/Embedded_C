// Вывести заданный массив размером N в обратном порядке.

#include <stdio.h>

#define N 7

int main()
{
    int a[N];
    int temp;

    for(int i = 0; i < N; i++)
    {
        a[i] = i + 1;
        printf("%d ", a[i]);
    }
    printf("\n");

    for(int i = 0; i < N/2; i++)
    {
        temp = a[i];
        a[i] = a[N - 1 - i];
        a[N - 1 - i] = temp;
    }

    for(int i = 0; i < N; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
