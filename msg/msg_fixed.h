#ifndef MSG_H
#define MSG_H

#define MAXTEXT 256
#define PERM 0666
#define KEY_FILE "/tmp/alpha_betta_queue.key"

#define ALPHA 1L
#define BETTA 2L

typedef struct {
    long mtype;              /* must be first */
    char mtext[MAXTEXT];
} AlphaMessage;

typedef struct {
    long mtype;              /* must be first */
    char mtext[MAXTEXT];
} BettaMessage;

#define MSG_BODY_SIZE(type) (sizeof(type) - sizeof(long))

#endif
