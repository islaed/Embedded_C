#include <stdio.h>
#include <unistd.h>


int main()
{
    char cwd[1024];
    
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    
    return 0;
}