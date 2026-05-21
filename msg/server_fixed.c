#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include "msg_fixed.h"

static int msqid = -1;

static void cleanup_and_exit(int code)
{
    if (msqid >= 0) {
        if (msgctl(msqid, IPC_RMID, NULL) < 0) {
            perror("Server: msgctl IPC_RMID");
        } else {
            printf("Server: message queue is deleted\n");
        }
    }
    exit(code);
}

static void sig_hndlr(int sig)
{
    (void)sig;
    cleanup_and_exit(0);
}

static int ensure_key_file(void)
{
    FILE *f = fopen(KEY_FILE, "a");
    if (f == NULL) {
        perror("Server: fopen key file");
        return -1;
    }
    fclose(f);
    return 0;
}

int main(void)
{
    AlphaMessage alpha;
    BettaMessage betta;
    key_t key;

    signal(SIGINT, sig_hndlr);

    if (ensure_key_file() < 0) {
        return 1;
    }

    key = ftok(KEY_FILE, 'A');
    if (key < 0) {
        perror("Server: ftok");
        return 1;
    }

    msqid = msgget(key, PERM | IPC_CREAT);
    if (msqid < 0) {
        perror("Server: msgget");
        return 1;
    }

    printf("Server: message queue is created\n");
    printf("Server: PID = %ld\n", (long)getpid());

    while (1) {
        ssize_t received = msgrcv(msqid, &alpha,
                                  MSG_BODY_SIZE(AlphaMessage),
                                  ALPHA, 0);
        if (received < 0) {
            perror("Server: msgrcv ALPHA");
            cleanup_and_exit(1);
        }

        printf("Server: received ALPHA: %s\n", alpha.mtext);

        betta.mtype = BETTA;
        snprintf(betta.mtext, MAXTEXT,
                 "BETTA from server, PID = %ld", (long)getpid());

        if (msgsnd(msqid, &betta, MSG_BODY_SIZE(BettaMessage), 0) < 0) {
            perror("Server: msgsnd BETTA");
            cleanup_and_exit(1);
        }

        printf("Server: sent BETTA: %s\n", betta.mtext);
    }
}
