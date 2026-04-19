#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t child_pid;
    int status;

    child_pid = fork();
    if(child_pid == 0)
    {
        printf("Child PID = %d, PPID = %d\n", getpid(), getppid());
        exit(1);
    }
    else
    {
        printf("Parent PID = %d\n", getpid());
        wait(&status);
        printf("Status = %d\n", WEXITSTATUS(status));
    }
    
    return 0;
}