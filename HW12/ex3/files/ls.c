#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int show_all = 0;    
    int show_full = 0; 
    char path[1024] = ".";

    // Получаем аргументы и путь
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            for(int j = 1; j < strlen(argv[i]); j++)
            {
                if(argv[i][j] == 'a')
                {
                    show_all = 1;
                }
                else if(argv[i][j] == 'l')
                {
                    show_full = 1;
                }
            }
        }
        else
        {
            strncpy(path, argv[i], sizeof(path) - 1);
        }
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Ошибка открытия папки!\n");
        return 1;
    }

    struct dirent *entry;
    struct stat file_stat;
    char full_file_path[2048];

    while((entry = readdir(dir)) != NULL)
    {
        // Условие для скрытых файлов
        if(!show_all && entry->d_name[0] == '.')
        {
            continue;
        }

        // Если есть аргумент -l, выводим информацию о файле
        if(show_full)
        {
            snprintf(full_file_path, sizeof(full_file_path), "%s/%s", path, entry->d_name);
            if(stat(full_file_path, &file_stat) == 0)
            {
                char time_str[20];
                struct tm *tm_info = localtime(&file_stat.st_mtime);
                strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm_info);

                printf("%ld %s ", file_stat.st_size, time_str);
            }
        }

        if(entry->d_type == DT_DIR)
        {
            printf("/%s\n", entry->d_name);
        }
        else
        {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}