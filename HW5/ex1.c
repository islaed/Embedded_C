#include <stdio.h>

#define M 100
#define N 21

struct abonent
{
    char name[N];
    char second_name[N];
    char tel[N];
};

char string_equality(char *name_1, char *name_2)
{
    int i = 0;
    
    while (name_1[i] != '\0' && name_2[i] != '\0')
    {
        if (name_1[i] != name_2[i])
        {
            return 0; // Строки не равны
        }
        i++;
    }

    // Проверяем, совпадают ли длины строк
    if (name_1[i] == '\0' && name_2[i] == '\0') 
    {
        return 1; // Строки равны
    } 
    else 
    {
        return 0; // Длины разные
    }
}

void add_abonent(char *abonents_flag, struct abonent *abonents)
{
    printf("Добавление абонента.");
    int counter = 0;
    for(int i = 0; i < M; i++)
    {
        if(abonents_flag[i] == 1)
        {
            counter++;
        }
    }
    if(counter == M)
    {
        printf("\nСписок абонентов переполнен!\n");
        return;
    }

    for(int i = 0; i < M; i++)
    {
        if(abonents_flag[i] == 0)
        {
            printf("\nВведите имя абонента: ");
            scanf("%s", abonents[i].name);
            printf("\nВведите фамилию абонента: ");
            scanf("%s", abonents[i].second_name);
            printf("\nВведите телефон абонента: ");
            scanf("%s", abonents[i].tel);
            abonents_flag[i] = 1;
            break;
        }
    }
}

void remove_abonent(char *abonents_flag, struct abonent *abonents)
{
    printf("Удаление абонента.\n");
    char flag = 0;
    char a_name[N];
    printf("Введите имя абонента, которого хотите удалить: ");
    scanf("%s", a_name);
    for(int i = 0; i < M; i++)
    {
        if(abonents_flag[i] == 1 && string_equality(abonents[i].name, a_name))
        {
            flag = 1;
            abonents_flag[i] = 0;
            printf("Удалён абонент: %s  %s  %s\n", abonents[i].name, abonents[i].second_name, abonents[i].tel);
            abonents[i] = (struct abonent){'0', '0', '0'};
        }
    }
    if(flag == 0)
    {
        printf("Абонента с таким именем не найдено!\n");
    }
}

void find_abonent(char *abonents_flag, struct abonent *abonents)
{
    printf("Поиск абонентов по имени.\n");
    char flag = 0;
    char a_name[N];
    printf("Введите имя абонента, которого хотите найти: ");
    scanf("%s", a_name);
    for(int i = 0; i < M; i++)
    {
        if(abonents_flag[i] == 1 && string_equality(abonents[i].name, a_name))
        {
            flag = 1;
            printf("%s  %s  %s\n", abonents[i].name, abonents[i].second_name, abonents[i].tel);
        }
    }
    if(flag == 0)
    {
        printf("Абонента с таким именем не найдено!\n");
    }
}

void show_abonents(char *abonents_flag, struct abonent *abonents)
{
    printf("Вывод всех записей.\n");
    char flag = 0;
    for(int i = 0; i < M; i++)
    {
        if(abonents_flag[i] == 1)
        {
            flag = 1;
            printf("%s  %s  %s\n", abonents[i].name, abonents[i].second_name, abonents[i].tel);
        }
    }
    if(flag == 0)
    {
        printf("Данных об абонентах нет!\n");
    }
}

int main()
{
    char number;
    char a_name[N];
    struct abonent abonents[M];

    // Создаём массив с флагами, чтобы знать какая позиция в массиве с абонентами занята
    char abonents_flag[M];
    for(int i = 0; i < M; i++)
    {
        abonents_flag[i] = 0;
    }

    while(1)
    {
        printf("\n1)Добавить абонента\n2)Удалить абонента\n3)Поиск абонентов по имени\n4)Вывод всех записей\n5)Выход\n\nВыберите пункт: ");
        scanf(" %c", &number);
        printf("\n");

        switch(number)
        {
            case '1':
                add_abonent(abonents_flag, abonents);
                break;

            case '2':
                remove_abonent(abonents_flag, abonents);
                break;

            case '3':
                find_abonent(abonents_flag, abonents);
                break;

            case '4':
                show_abonents(abonents_flag, abonents);
                break;

            case '5':
                printf("Выход из программы.\n");
                return 0;

            default:
                printf("Такого пункта нет!\n");
                break;
        }
    }
}