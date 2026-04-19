#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main(int arguments_counter, char *arguments[])
{
    DIR *dir;
    if(arguments[1] == NULL)
    {
        dir = opendir(".");
    }
    else
    {
        char full_path[1024] = "";
        // Склеиваем аргументы для папок, у которых в названии есть пробел
        for(int i = 1; i < arguments_counter; i++)
        {
            strcat(full_path, arguments[i]);
            if(i < arguments_counter - 1)
            {
                strcat(full_path, " ");
            }
        }
        dir = opendir(full_path);
    }

    struct dirent *entry;

    if(dir == NULL)
    {
        printf("Ошибка открытия папки!\n");
        return 1;
    }
    
    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_name[0] != '.')
        {
            if(entry->d_type == DT_DIR) 
                printf("/%s\n", entry->d_name);
            else
                printf("%s\n", entry->d_name);
        }
    }
    printf("\n");
    closedir(dir);
    
    return 0;
}