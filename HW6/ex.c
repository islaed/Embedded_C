#include <stdio.h>
#include <stdlib.h>

#define N 21

// Структура, где будут хранится данные абонента, а так же адреса следующего и предыдущего абонентов
typedef struct abonent_node
{
    struct abonent_node *next;
    struct abonent_node *prev;

    char name[N];
    char second_name[N];
    char tel[N];
} abonent_node;

// Структура, где будут хранится адреса первого и последнего элемента в списке
typedef struct abonent_list
{
    abonent_node *head;
    abonent_node *tail;
} abonent_list;

// Инициализация пустого списка
void init_list(abonent_list *list)
{
    list->head = NULL;
    list->tail = NULL;
}

// Добавление абонента в список
void add_abonent(abonent_list *list)
{
    // Выделяем память под нового абонента
    abonent_node *new_abonent = (abonent_node*)malloc(sizeof(abonent_node));

    printf("\nВведите имя абонента: ");
    scanf("%s", new_abonent->name);
    printf("\nВведите фамилию абонента: ");
    scanf("%s", new_abonent->second_name);
    printf("\nВведите телефон абонента: ");
    scanf("%s", new_abonent->tel);

    new_abonent->next = NULL;
    new_abonent->prev = list->tail;

    if(list->tail == NULL) // Если список пуст
    {
        list->head = new_abonent;
        list->tail = new_abonent;
    }
    else
    {
        list->tail->next = new_abonent;
        list->tail = new_abonent;
    }
}

// Функция для проверки одинаковых строк
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

// Поиск абонентов по имени
void find_abonent(abonent_list *list)
{
    printf("Поиск абонентов по имени.\n");
    int flag = 0;
    char a_name[N];
    printf("Введите имя абонента, которого хотите найти: ");
    scanf("%s", a_name);

    for(abonent_node *current_ab = list->head; current_ab != NULL; current_ab = current_ab->next)
    {
        if (string_equality(current_ab->name, a_name))
        {
            flag++;
            printf("Имя:%s   Фамилия:%s   Телефон:%s\n", current_ab->name, current_ab->second_name, current_ab->tel);
        }
    }
    if(!flag) printf("Абонентов с таким именем не найдено!\n");
}

// Вывод всех абонентов
void show_abonents(abonent_list *list)
{
    for(abonent_node *current_ab = list->head; current_ab != NULL; current_ab = current_ab->next)
    {
        printf("Имя:%s   Фамилия:%s   Телефон:%s\n", current_ab->name, current_ab->second_name, current_ab->tel);
    }
}

// Удаление абонентов из списка
void remove_abonent(abonent_list *list)
{
    printf("Удаление абонентов по имени.\n");
    char a_name[N];
    int flag = 0;
    printf("Введите имя абонента, которого хотите удалить: ");
    scanf("%s", a_name);

    abonent_node *current_ab = list->head;
    abonent_node *next_ab;
    while(current_ab != NULL)
    {
        next_ab = current_ab->next;
        if(string_equality(current_ab->name, a_name))
        {
            flag++;
            if(current_ab->prev == NULL) // Если текущий абонент в начале списка
            {
                list->head = current_ab->next;
                current_ab->next->prev = NULL;
            }
            else if(current_ab->next == NULL) // Если текущий абонент в конце списка
            {
                list->tail = current_ab->prev;
                current_ab->prev->next = NULL;
            }
            else // Если абонент не в начале и не в конце
            {
                current_ab->prev->next = current_ab->next;
                current_ab->next->prev = current_ab->prev;
            }
            printf("Удалён абонент: %s  %s  %s\n", current_ab->name, current_ab->second_name, current_ab->tel);
            free(current_ab);
        }
        current_ab = next_ab;
    }
    if(!flag) printf("Абонентов с таким именем не найдено!");
}

// Функция завершения программы
void close_program(abonent_list *list)
{
    printf("Завершение работы.\n");
    abonent_node *current_ab = list->head;
    abonent_node *next_ab;
    while(current_ab != NULL)
    {
        next_ab = current_ab->next;
        free(current_ab);
        current_ab = next_ab;
    }
    exit(0);
}

int main()
{
    char number;
    abonent_list list;
    init_list(&list);

    while(1)
    {
        printf("\n1)Добавить абонента\n2)Удалить абонента\n3)Поиск абонентов по имени\n4)Вывод всех записей\n5)Выход\n\nВыберите пункт: ");
        scanf(" %c", &number);
        printf("\n");

        switch(number)
        {
            case '1':
                add_abonent(&list);
                break;
            case '2':
                remove_abonent(&list);
                break;
            case '3':
                find_abonent(&list);
                break;
            case '4':
                show_abonents(&list);
                break;
            case '5':
                close_program(&list);
                break;
            default:
                printf("Такого пункта нет!");
                break;
        }
    }
    return 0;
}