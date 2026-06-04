#define _GNU_SOURCE // без него siginfo_t, sigset_t и прочие подчеркиваются как ошибки
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

int main(void)
{
    sigset_t set;
    int ret;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    
    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if(ret < 0)
    {
        perror("Cant set signal handler!\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}