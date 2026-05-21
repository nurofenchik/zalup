#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include "msg_fixed.h"

int main(void)
{
    AlphaMessage alpha;
    BettaMessage betta;
    key_t key;
    int msqid;

    key = ftok(KEY_FILE, 'A');
    if (key < 0) {
        perror("Client: ftok. Start server first");
        return 1;
    }

    msqid = msgget(key, PERM);
    if (msqid < 0) {
        perror("Client: msgget. Start server first");
        return 1;
    }

    alpha.mtype = ALPHA;
    snprintf(alpha.mtext, MAXTEXT,
             "ALPHA from client, PID = %ld", (long)getpid());

    if (msgsnd(msqid, &alpha, MSG_BODY_SIZE(AlphaMessage), 0) < 0) {
        perror("Client: msgsnd ALPHA");
        return 1;
    }

    printf("Client: sent ALPHA: %s\n", alpha.mtext);

    if (msgrcv(msqid, &betta, MSG_BODY_SIZE(BettaMessage), BETTA, 0) < 0) {
        perror("Client: msgrcv BETTA");
        return 1;
    }

    printf("Client: received BETTA: %s\n", betta.mtext);

    return 0;
}
