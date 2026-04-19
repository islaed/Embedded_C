#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    int status;

    pid_t proc1 = fork();
    if(proc1 == 0)
    {
        printf("Process 1 PID = %d, PPID = %d\n", getpid(), getppid());
        
        pid_t proc3 = fork();
        if(proc3 == 0)
        {
            printf("Process 3 PID = %d, PPID = %d\n", getpid(), getppid());
            exit(3);
        }
        
        pid_t proc4 = fork();
        if(proc4 == 0)
        {
            printf("Process 4 PID = %d, PPID = %d\n", getpid(), getppid());
            exit(4);
        }
        
        waitpid(proc3, &status, 0);
        printf("Status exit = %d\n", WEXITSTATUS(status));

        waitpid(proc4, &status, 0);
        printf("Status exit = %d\n", WEXITSTATUS(status)); 

        exit(1);
    }
    
    pid_t proc2 = fork();
    if(proc2 == 0)
    {
        printf("Process 2 PID = %d, PPID = %d\n", getpid(), getppid());
        
        pid_t proc5 = fork();
        if(proc5 == 0)
        {
            printf("Process 5 PID = %d, PPID = %d\n", getpid(), getppid());
            exit(5);
        }

        waitpid(proc5, &status, 0);
        printf("Status exit = %d\n", WEXITSTATUS(status));

        exit(2);
    }

    waitpid(proc1, &status, 0);
    printf("Status exit = %d\n", WEXITSTATUS(status));

    waitpid(proc2, &status, 0);
    printf("Status exit = %d\n", WEXITSTATUS(status));

    return 0;
}