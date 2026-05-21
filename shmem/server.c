#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "shmem.h"

Message *msgptr;
int shmid;
int semid;

void hndlr(int sig)
{
    signal(SIGINT, hndlr);

    if (shmdt(msgptr) < 0) {
        printf("Server: error of detaching shared memory\n");
        exit(-1);
    }

    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        printf("Server: can't delete shared memory area\n");
        exit(-1);
    }
    printf("Server: shared memory area is deleted\n");

    if (semctl(semid, 0, IPC_RMID) < 0) {
        printf("Server: can't delete semaphores\n");
        exit(-1);
    }
    printf("Server: semaphores are deleted\n");

    exit(0);
}

int main(void)
{
    key_t key;

    signal(SIGINT, hndlr);

    if ((key = ftok("test.txt", 'A')) < 0) {
        printf("Server: can't get a key\n");
        return 1;
    }

    if ((shmid = shmget(key, sizeof(Message), PERM | IPC_CREAT)) < 0) {
        printf("Server: can't create shared memory area\n");
        exit(-1);
    }
    printf("Server: shared memory area is created\n");

    msgptr = (Message *)shmat(shmid, 0, 0);
    if ((long)msgptr == -1) {
        printf("Server: error of joining shared memory\n");
        exit(-1);
    }
    printf("Server: shared memory area is joined\n");

    if ((semid = semget(key, 2, PERM | IPC_CREAT)) < 0) {
        printf("Server: can't create semaphores\n");
        exit(-1);
    }
    printf("Server: semaphores are created\n");
    printf("Server: PID = %ld\n", (long)getpid());

    while (1) {
        if (semop(semid, &proc_wait[0], 1) < 0) {
            printf("Server: execution complete\n");
            exit(-1);
        }

        if (semop(semid, &mem_lock[0], 2) < 0) {
            printf("Server: can't execute operation with semaphores\n");
            exit(-1);
        }

        printf("Server: read message\n%s", msgptr->buff);

        sprintf(msgptr->buff, "Message from server with PID = %ld\n", (long)getpid());

        if (semop(semid, &mem_unlock[0], 1) < 0) {
            printf("Server: can't execute operation with semaphores\n");
            exit(-1);
        }
    }

}
