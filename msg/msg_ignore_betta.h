#ifndef MSG_IGNORE_BETTA_H
#define MSG_IGNORE_BETTA_H

#define MAXTEXT 256
#define PERM 0666
#define KEY_FILE "/tmp/alpha_betta_queue.key"
#define ALPHA_LOG_FILE "alpha_messages.txt"

#define ALPHA 1L
#define BETTA 2L

typedef struct {
    long mtype;
    char mtext[MAXTEXT];
} AlphaMessage;

typedef struct {
    long mtype;
    char mtext[MAXTEXT];
} BettaMessage;

#define MSG_BODY_SIZE(type) (sizeof(type) - sizeof(long))

#endif
