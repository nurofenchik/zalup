#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "shmem.h"

int main(void)
{
    Message *msgptr;
    key_t key;
    int shmid;
    int semid;

    if ((key = ftok("test.txt", 'A')) < 0) {
        printf("Client: can't get a key\n");
        exit(-1);
    }

    if ((shmid = shmget(key, sizeof(Message), 0)) < 0) {
        printf("Client: access to shared memory is denied\n");
        exit(-1);
    }

    msgptr = (Message *)shmat(shmid, 0, 0);
    if ((long)msgptr == -1) {
        printf("Client: error of joining shared memory\n");
        exit(-1);
    }

    if ((semid = semget(key, 2, PERM)) < 0) {
        printf("Client: access to semaphores is denied\n");
        exit(-1);
    }

    if (semop(semid, &mem_lock[0], 2) < 0) {
        printf("Client: can't execute operation with semaphores\n");
        exit(-1);
    }

    if (semop(semid, &proc_start[0], 1) < 0) {
        printf("Client: can't execute operation with semaphores\n");
        exit(-1);
    }

    sprintf(msgptr->buff, "Message from client with PID = %ld\n", (long)getpid());

    if (semop(semid, &mem_unlock[0], 1) < 0) {
        printf("Client: can't execute operation with semaphores\n");
        exit(-1);
    }

    if (semop(semid, &mem_lock[0], 2) < 0) {
        printf("Client: can't execute operation with semaphores\n");
        exit(-1);
    }

    printf("Client: read message\n%s", msgptr->buff);

    if (semop(semid, &mem_unlock[0], 1) < 0) {
        printf("Client: can't execute operation with semaphores\n");
        exit(-1);
    }

    if (shmdt(msgptr) < 0) {
        printf("Client: error of detaching shared memory\n");
        exit(-1);
    }
}
