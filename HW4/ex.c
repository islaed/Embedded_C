#include <stdio.h>

#define M 100
#define N 10

struct abonent
{
    char name[N];
    char second_name[N];
    char tel[N];
};

int main()
{
    char number;
    struct abonent abonents[M];

    abonents[0] = (struct abonent){"Илья", "secname", "12345"};
    abonents[1] = (struct abonent){"Gasd", "tgdfname", "54321"};

    while(1)
    {
        printf("1)Добавить абонента\n2)Удалить абонента\n3)Поиск абонентов по имени\n4)Вывод всех записей\n5)Выход\n\nВыберите пункт: ");
        scanf(" %c", &number);

        switch(number)
        {
        case '1':
            printf("Пункт 1\n\n");
            break;
        case '2':
            printf("Пункт 2\n\n");
            break;
        case '3':
            printf("Пункт 3\n\n");
            break;
        case '4':
            printf("Пункт 4\n\n");
            break;
        case '5':
            printf("Выход из программы.\n");
            return 0;
        default:
            printf("Такого пункта нет!\n\n");
            break;
        }
    }
}