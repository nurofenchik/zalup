#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "msg.h"

int main(void)
{
    Message msg_snd;
    Message msg_rcv;
    key_t key;
    int msqid;
    int length;

    if ((key = ftok("test.txt", 'A')) < 0) {
        printf("Client: can't receive a key\n");
        return 1;
    }

    if ((msqid = msgget(key, PERM)) < 0) {
        printf("Client: can't access message queue\n");
        return 1;
    }

    msg_snd.mtype = 1;
    snprintf(msg_snd.mtext, MAXTEXT,
             "Message from client, PID = %ld", (long)getpid());

    length = strlen(msg_snd.mtext) + 1;

    if (msgsnd(msqid, &msg_snd, length, 0) < 0) {
        printf("Client: can't send message\n");
        return 1;
    }

    printf("Client: sent message: %s\n", msg_snd.mtext);

    if (msgrcv(msqid, &msg_rcv, MAXTEXT, 2, 0) < 0) {
        printf("Client: can't receive message\n");
        return 1;
    }

    printf("Client: received reply: %s\n", msg_rcv.mtext);

    return 0;
}
