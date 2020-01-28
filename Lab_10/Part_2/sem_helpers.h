#pragma once

#include <sys/sem.h>

void set_simple_semaphore_value(key_t sem_id, ushort sem_num, int value);
void increment_simple_semaphore(key_t sem_id, ushort sem_num);
void change_simple_semaphore(key_t sem_id, ushort sem_num, short increment_amount);
void decrement_simple_semaphore(key_t sem_id, ushort sem_num);
void wait_fo_zero(key_t sem_id, ushort sem_num);