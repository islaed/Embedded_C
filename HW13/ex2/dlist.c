#include "dlist.h"

// Инициализация пустого списка клиентов
void init_list(client_list *list)
{
    list->head = NULL;
    list->tail = NULL;
}

// Добавление клиента
void add_abonent(client_list *list, char *client_name)
{
    // Выделяем память под нового клиента
    client_node *new_client = (client_node*)malloc(sizeof(client_node));

    printf("\nВведите имя абонента: ");
    scanf("%s", new_client->name);

    // Записываем имя клиента
    strcpy(new_client->name, client_name);

    new_client->next = NULL;
    new_client->prev = list->tail;

    if(list->tail == NULL) // Если список пуст
    {
        list->head = new_client;
        list->tail = new_client;
    }
    else
    {
        list->tail->next = new_client;
        list->tail = new_client;
    }
}

// Поиск клиентов по имени
void find_client(client_list *list)
{
    printf("Поиск абонентов по имени.\n");
    int flag = 0;
    char a_name[NAME_SIZE];
    printf("Введите имя абонента, которого хотите найти: ");
    scanf("%s", a_name);

    for(client_node *current_client = list->head; current_client != NULL; current_client = current_client->next)
    {
        if(string_equality(current_client->name, a_name))
        {
            flag++;
        }
    }
    if(!flag) printf("Абонентов с таким именем не найдено!\n");
}

// Вывод всех абонентов
void show_clients(client_list *list)
{
    for(client_node *current_client = list->head; current_client != NULL; current_client = current_client->next)
    {
        printf("Имя:%s", current_client->name);
    }
}