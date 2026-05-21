#ifndef MSG_H
#define MSG_H

#define MAXTEXT 256
#define PERM 0666

typedef struct
{
    long mtype;
    char mtext[MAXTEXT];
} Message;

#endif
