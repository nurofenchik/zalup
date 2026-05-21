#ifndef SHMEM_H
#define SHMEM_H

#define MAXBUFF 80
#define PERM 0666

typedef struct mem
{
    int segment;
    char buff[1024UL*1024UL*1024UL*2UL];
} Message;

/* ожидание начала выполнения клиента */
static struct sembuf proc_wait[1] = {{1, -1, 0}};

/* уведомление сервера о том, что клиент начал работу */
static struct sembuf proc_start[1] = {{1, 1, 0}};

/* блокирование разделяемой памяти */
static struct sembuf mem_lock[2] = {{0, 0, 0}, {0, 1, 0}};

/* освобождение ресурса */
static struct sembuf mem_unlock[1] = {{0, -1, 0}};

#endif
