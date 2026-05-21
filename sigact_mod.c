#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void handler(int sig);

void (*mysig(int sig, void (*handler_func)(int)))(int)
{
    struct sigaction act;
    struct sigaction oldact;

    act.sa_handler = handler_func;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = 0;

    if (sigaction(sig, &act, &oldact) < 0) {
        return SIG_ERR;
    }

    return oldact.sa_handler;
}

void handler(int sig)
{
    if (sig == SIGUSR1) {
        printf("sigact_selfkill: received SIGUSR1 in process %ld\n", (long)getpid());
    } else if (sig == SIGUSR2) {
        printf("sigact_selfkill: received SIGUSR2 in process %ld\n", (long)getpid());
    }

    printf("sigact_selfkill: sending SIGINT to itself from handler\n");
    kill(getpid(), SIGINT);

    printf("sigact_selfkill: SIGINT is blocked until handler exits\n");
    sleep(5);
    printf("sigact_selfkill: handler finished, pending SIGINT can now be delivered\n");
}

int main(void)
{
    struct sigaction act_int;

    if (mysig(SIGUSR1, handler) == SIG_ERR) {
        perror("sigact_selfkill: mysig(SIGUSR1)");
        return 1;
    }

    if (mysig(SIGUSR2, handler) == SIG_ERR) {
        perror("sigact_selfkill: mysig(SIGUSR2)");
        return 1;
    }

    act_int.sa_handler = SIG_DFL;
    sigemptyset(&act_int.sa_mask);
    act_int.sa_flags = 0;

    if (sigaction(SIGINT, &act_int, 0) < 0) {
        perror("sigact_selfkill: sigaction(SIGINT)");
        return 1;
    }

    printf("sigact_selfkill: PID = %ld\n", (long)getpid());
    printf("sigact_selfkill: send SIGUSR1 or SIGUSR2 from another terminal\n");

    while (1) {
        pause();
    }

    return 0;
}
