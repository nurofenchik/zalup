#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "msg_ignore_betta.h"

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

static int append_alpha_to_file(const AlphaMessage *alpha)
{
    FILE *f = fopen(ALPHA_LOG_FILE, "a");
    if (f == NULL) {
        perror("Server: fopen alpha log file");
        return -1;
    }

    if (fprintf(f, "%s\n", alpha->mtext) < 0) {
        perror("Server: fprintf alpha log file");
        fclose(f);
        return -1;
    }

    if (fclose(f) != 0) {
        perror("Server: fclose alpha log file");
        return -1;
    }

    return 0;
}

int main(void)
{
    AlphaMessage alpha;
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

        if (append_alpha_to_file(&alpha) < 0) {
            cleanup_and_exit(1);
        }

        /* BETTA is intentionally ignored: server does not send it. */
    }
}
