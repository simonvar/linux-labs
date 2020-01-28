#include "sem_helpers.h"

#include <sys/types.h>
#include <sys/ipc.h>

void set_simple_semaphore_value(key_t sem_id, ushort sem_num, int value) {
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *__buf;
    } arg;
    arg.val = value;
    semctl(sem_id, sem_num, SETVAL, arg);
}

void increment_simple_semaphore(key_t sem_id, ushort sem_num) {
    change_simple_semaphore(sem_id, sem_num, 1);
}

void change_simple_semaphore(key_t sem_id, ushort sem_num, short increment_amount) {
    sembuf ops {
        sem_num,
        increment_amount,
        0
    };
    semop(sem_id, &ops, 1);
}

void decrement_simple_semaphore(key_t sem_id, ushort sem_num) {
    change_simple_semaphore(sem_id, sem_num, -1);
}

void wait_fo_zero(key_t sem_id, ushort sem_num) {
    sembuf ops {
        sem_num,
        0,
        0
    };
    semop(sem_id, &ops, 1);
}