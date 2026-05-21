#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "msg.h"

int msqid = -1;

void sig_hndlr(int sig)
{
    signal(SIGINT, sig_hndlr);

    if (msqid >= 0) {
        if (msgctl(msqid, IPC_RMID, 0) < 0) {
            printf("Server: can't delete message queue\n");
            exit(1);
        }
        printf("Server: message queue is deleted\n");
    }

    exit(0);
}

int main(void)
{
    Message msg_rcv;
    Message msg_snd;
    key_t key;
    int length;

    signal(SIGINT, sig_hndlr);

    if ((key = ftok("test.txt", 'A')) < 0) {
        printf("Server: can't receive a key\n");
        return 1;
    }

    if ((msqid = msgget(key, PERM | IPC_CREAT)) < 0) {
        printf("Server: can't create message queue\n");
        return 1;
    }

    printf("Server: message queue is created\n");
    printf("Server: PID = %ld\n", (long)getpid());

    while (1) {
        if (msgrcv(msqid, &msg_rcv, MAXTEXT, 1, 0) < 0) {
            printf("Server: can't receive message\n");
            kill(getpid(), SIGINT);
            return 1;
        }

        printf("Server: received message: %s\n", msg_rcv.mtext);

        msg_snd.mtype = 2;
        snprintf(msg_snd.mtext, MAXTEXT,
                 "Reply from server, PID = %ld", (long)getpid());

        length = strlen(msg_snd.mtext) + 1;

        if (msgsnd(msqid, &msg_snd, length, 0) < 0) {
            printf("Server: can't send message\n");
            kill(getpid(), SIGINT);
            return 1;
        }

        printf("Server: sent reply: %s\n", msg_snd.mtext);
    }

    return 0;
}
