#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void handler(int sig);

void (*mysig(int sig, void (*handler_func)(int)))(int)
{
    struct sigaction act;

    act.sa_handler = handler_func;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = 0; // SA_RESETHAND;

    if (sigaction(sig, &act, 0) < 0) {
        return SIG_ERR;
    }

    return act.sa_handler;
}

void handler(int sig)
{
    if (sig == SIGUSR1) {
        printf("sigact_block: received SIGUSR1 in process %ld\n", (long)getpid());
    } else if (sig == SIGUSR2) {
        printf("sigact_block: received SIGUSR2 in process %ld\n", (long)getpid());
    }

    printf("sigact_block: SIGINT is blocked for 60 seconds inside handler\n");
    sleep(20);
    printf("sigact_block: handler finished\n");
}

int main(void)
{
    if (mysig(SIGUSR1, handler) == SIG_ERR) {
        perror("sigact_block: mysig(SIGUSR1)");
        return 1;
    }

    if (mysig(SIGUSR2, handler) == SIG_ERR) {
        perror("sigact_block: mysig(SIGUSR2)");
        return 1;
    }
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
	perror("sigact_block: SIGINT");
	return 1;
    }
    printf("sigact_block: PID = %ld\n", (long)getpid());
    printf("sigact_block: send SIGUSR1 or SIGUSR2, then try SIGINT from another terminal\n");

    while (1) {
        pause();
	printf("SIGNAL CATCHED\n");
    }

    return 0;
}
