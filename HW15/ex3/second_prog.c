#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    printf("Введите pid: ");
    scanf("%d", &pid);

    kill(pid, SIGUSR1);

    return 0;
}