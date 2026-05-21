#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t parent_pid;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <parent_pid>\n", argv[0]);
        return 1;
    }

    parent_pid = (pid_t)strtol(argv[1], NULL, 10);

    printf("sig_child: parent PID = %ld\n", (long)parent_pid);

    if (kill(parent_pid, SIGUSR1) == 0) {
        printf("sig_child: SIGUSR1 sent successfully to parent PID = %ld\n",
               (long)parent_pid);
    } else {
        fprintf(stderr,
                "sig_child: failed to send SIGUSR1 to parent PID = %ld: %s\n",
                (long)parent_pid, strerror(errno));
        return 1;
    }
    return 0;
}
