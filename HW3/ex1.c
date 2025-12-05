// Поменять в целом положительном числе (типа int) значение третьего 
// байта на введенное пользователем число (изначальное число также 
// вводится с клавиатуры) через указатель (не применяя битовые 
// операции).

#include <stdio.h>

int main()
{
    unsigned int number;
    unsigned char new_byte;
    unsigned char* ptr;

    printf("Введите целое положительное число: ");
    scanf("%d", &number);
    printf("Введите число от 0 до 255, на которое хотите заменить 3-й байт: ");
    scanf("%hhu", &new_byte);
    
    ptr = (unsigned char*)&number;

    ptr = ptr + 2;
    *ptr = new_byte;

    printf("Число с изменённым 3-м байтом: %d\n", number);
    
    return 0;
}