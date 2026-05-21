#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

static void usr_handler(int sig)
{
    pid_t parent_pid = getpid();

    if (signal(sig, usr_handler) == SIG_ERR) {
        perror("sig_parent: signal");
    }

    if (sig == SIGUSR1) {
        printf("sig_parent: successfully received SIGUSR1, parent PID = %ld\n",
               (long)parent_pid);
    } else if (sig == SIGUSR2) {
        printf("sig_parent: successfully received SIGUSR2, parent PID = %ld\n",
               (long)parent_pid);
    } else {
        printf("sig_parent: received signal %d, parent PID = %ld\n",
               sig, (long)parent_pid);
    }

}

int main(void)
{
    pid_t pid;

    if (signal(SIGUSR1, usr_handler) == SIG_ERR) {
        perror("sig_parent: signal(SIGUSR1)");
        return 1;
    }

    if (signal(SIGUSR2, usr_handler) == SIG_ERR) {
        perror("sig_parent: signal(SIGUSR2)");
        return 1;
    }

    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("sig_parent: signal(SIGINT)");
        return 1;
    }

    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
        perror("sig_parent: signal(SIGCHLD)");
        return 1;
    }

    printf("sig_parent: parent PID = %ld\n", (long)getpid());

    pid = fork();
    if (pid < 0) {
        perror("sig_parent: fork");
        return 1;
    }

    if (pid == 0) {
        char ppid_str[32];
        snprintf(ppid_str, sizeof(ppid_str), "%ld", (long)getppid());
        execl("./sig_child", "sig_child", ppid_str, (char *)NULL);

        perror("sig_parent: execl");
        exit(1);
    }

    printf("sig_parent: child PID = %ld\n", (long)pid);
    printf("sig_parent: waiting for signals...\n");
    printf("Use from another terminal: kill -s SIGUSR1 %ld\n", (long)getpid());
    printf("                         kill -s SIGUSR2 %ld\n", (long)getpid());
    printf("                         kill -s SIGINT  %ld\n", (long)getpid());

    while (1) {
        pause();
    }

    return 0;
}
