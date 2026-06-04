#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    printf("Введите pid: ");
    scanf("%d", &pid);

    kill(pid, SIGINT);

    return 0;
}