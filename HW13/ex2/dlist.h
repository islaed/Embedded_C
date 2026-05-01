#ifndef DLIST_H
#define DLIST_H


#include <stdio.h>
#include <string.h>


#define NAME_SIZE 256

typedef struct client_node
{
    struct client_node *next;
    struct client_node *prev;

    char name[NAME_SIZE];
} client_node;

typedef struct client_list
{
    client_node *head;
    client_node *tail;
} client_list;

void init_list(client_list *list);
void add_abonent(client_list *list, char *client_name);

#endif