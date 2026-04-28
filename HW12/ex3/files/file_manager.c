#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>


void print_dir(WINDOW *window, struct dirent **dir_list, int n, int pos, int is_active, int height, int max_name_width, int offset)
{
    int file_index;

    for(int i = 1; i < height - 1; i++)
    {
        file_index = i + offset;

        if(file_index >= n)
            break;

        if(is_active && file_index == pos)
            wattron(window, A_REVERSE);

        char *name = dir_list[file_index]->d_name;
        int name_len = strlen(name);
        int print_width = max_name_width - 2;

        // Сокращение имени файла
        if(name_len > print_width && print_width > 5)
        {
            int half_name = (print_width - 1) / 2; // Половина имени файла
            char short_file_name[1024];

            strncpy(short_file_name, name, half_name);
            short_file_name[half_name] = '\0';
            strcat(short_file_name, "~");
            strcat(short_file_name, name + name_len - (print_width - half_name - 1));

            if(dir_list[file_index]->d_type == DT_DIR)
                mvwprintw(window, i, 1, "/%s", short_file_name);
            else
                mvwprintw(window, i, 1, " %s", short_file_name);
        }
        else
        {
            if(dir_list[file_index]->d_type == DT_DIR)
                mvwprintw(window, i, 1, "/%s", name);
            else
                mvwprintw(window, i, 1, " %s", name);
        }

        wattroff(window, A_REVERSE);
    }
}

// Функция вывода пути к текущей папке
void print_path(WINDOW *window, char *current_dir, int max_length)
{
    wattron(window, A_REVERSE);
    if(strlen(current_dir) > max_length)
    {
        // Сокращение пути к папке
        char *short_path_left = current_dir + (strlen(current_dir) - max_length + 2);
        mvwprintw(window, 0, 2, "...%s", short_path_left);
    }
    else
    {
        mvwprintw(window, 0, 2, "%s", current_dir);
    }
    wattroff(window, A_REVERSE);
}

int main()
{
    setlocale(LC_ALL, ""); // Чтобы были видны русские буквы
    
    initscr();
    noecho(); // Чтобы нельзя было писать
    curs_set(0); // Убираем видимость курсора
    keypad(stdscr, TRUE); // Включаем управление стрелочками

    // Выставляем черный и белый цвет
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    attron(COLOR_PAIR(1));

    int height, width, y_begin, x_begin;
    int y_pos = 1, x_pos = 1;
    int current_window = 0;
    int key;
    int max_length;
    int max_name_width;

    int left_offset = 0;
    int right_offset = 0;

    struct dirent **left_dir_list, **right_dir_list;
    int left_dir, right_dir;

    // Получаем путь к текущей папке
    char *current_dir_left = getcwd(NULL, 0);
    char *current_dir_right = getcwd(NULL, 0);

    WINDOW *window_left, *window_right;

    while(1)
    {
        getbegyx(stdscr, y_begin, x_begin);
        getmaxyx(stdscr, height, width); // Получаем значения высоты и ширины окна
        height--;
        window_left = newwin(height, width / 2, y_begin, x_begin);
        window_right = newwin(height, width / 2, y_begin, x_begin + width / 2);
        max_length = (width / 2) - 10; // Максимальная длина строки до сокращения
        max_name_width = (width / 2) - 4;

        box(window_left, 0, 0);
        box(window_right, 0, 0);

        // Получаем списки файлов в директории и сразу сортируем
        left_dir = scandir(current_dir_left, &left_dir_list, NULL, alphasort);
        right_dir = scandir(current_dir_right, &right_dir_list, NULL, alphasort);

        print_dir(window_left, left_dir_list, left_dir, y_pos, (!current_window), height, max_name_width, left_offset);
        print_dir(window_right, right_dir_list, right_dir, x_pos, (current_window), height, max_name_width, right_offset);

        print_path(window_left, current_dir_left, max_length);
        print_path(window_right, current_dir_right, max_length);

        mvprintw(height, 0, "ENTER:Вход в директорию | TAB:Переключение панелей | BACKSPACE:Выход");

        // Обновление всех окон
        refresh();
        wrefresh(window_left);
        wrefresh(window_right);
        
        key = getch();
        switch(key)
        {
            case KEY_UP:
                if(!current_window)
                {
                    y_pos--;
                    if(y_pos <= 0)
                        y_pos = 1;
                }
                else
                {
                    x_pos--;
                    if(x_pos <= 0)
                        x_pos = 1;
                }
                break;

            case KEY_DOWN:
                if(!current_window)
                {
                    y_pos++;
                    if(y_pos >= left_dir)
                        y_pos = left_dir - 1;
                }
                else
                {
                    x_pos++;
                    if(x_pos >= right_dir)
                        x_pos = right_dir - 1;
                }
                break;
            
            // Вход в директорию
            case '\n': // Enter
                if(!current_window)
                {
                    if(left_dir_list[y_pos]->d_type == DT_DIR)
                    {
                        if(chdir(left_dir_list[y_pos]->d_name) == 0)
                        {
                            free(current_dir_left);
                            current_dir_left = getcwd(NULL, 0);
                            y_pos = 1;
                            left_offset = 0;
                        }
                    }
                }
                else
                {
                    if(right_dir_list[x_pos]->d_type == DT_DIR)
                    {
                        if(chdir(right_dir_list[x_pos]->d_name) == 0)
                        {
                            free(current_dir_right);
                            current_dir_right = getcwd(NULL, 0);
                            x_pos = 1;
                            right_offset = 0;
                        }
                    }
                }
                break;
            
            // Переключение окна
            case '\t': // Tab
                if(!current_window)
                    chdir(current_dir_right);

                else
                    chdir(current_dir_left);

                current_window = !current_window;            
                break;    

            // Полная очистка памяти и завершение программы
            case KEY_BACKSPACE:
                free(current_dir_left);
                free(current_dir_right);
                
                for(int i = 0; i < left_dir; i++)
                    free(left_dir_list[i]);
                free(left_dir_list);

                for(int i = 0; i < right_dir; i++)
                    free(right_dir_list[i]);
                free(right_dir_list);

                delwin(window_left);
                delwin(window_right);
                endwin();
                return 0;
            
            default:
                break;
        }

        int visible_lines = height - 2; // Видимые строки

        if(!current_window) // Левая панель
        {
            // Если курсор ушел выше
            if(y_pos < left_offset + 1)
                left_offset = y_pos - 1;

            // Если курсор ушел ниже
            if(y_pos > left_offset + visible_lines)
                left_offset = y_pos - visible_lines;

            // Чтобы курсор не ушел выше начала списка
            if(left_offset < 0)
                left_offset = 0;

            // Чтобы курсор не ушел ниже конца списка
            if(left_dir > visible_lines && left_offset > left_dir - visible_lines)
                left_offset = left_dir - visible_lines;
        }
        else // Правая панель
        {
            if(x_pos < right_offset + 1)
                right_offset = x_pos - 1;

            if(x_pos > right_offset + visible_lines)
                right_offset = x_pos - visible_lines;

            if(right_offset < 0)
                right_offset = 0;

            if(right_dir > visible_lines && right_offset > right_dir - visible_lines)
                right_offset = right_dir - visible_lines;
        }

        // Очистка памяти
        for(int i = 0; i < left_dir; i++)
            free(left_dir_list[i]);
        free(left_dir_list);

        for(int i = 0; i < right_dir; i++)
            free(right_dir_list[i]);
        free(right_dir_list);

        delwin(window_left);
        delwin(window_right);
    }   

    endwin();
    return 0;
}