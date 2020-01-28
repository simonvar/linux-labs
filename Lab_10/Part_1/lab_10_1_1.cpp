#include <iostream>
#include <fstream>

#include <sys/sem.h>

#include "shmem_helpers.h"
#include "sem_helpers.h"

constexpr unsigned BUFFER_LENGTH {3};

constexpr key_t BUFFER_SHMEM_KEY {0x8d};
constexpr key_t BUFFER_LENGTH_SHMEM_KEY {0x8e};
// constexpr key_t THERE_WILL_BE_NO_MORE_DATA_SHMEM_KEY {0x8f};
constexpr auto FILE_PATH {"in"};

constexpr key_t SEM_KEY {0x90};
constexpr short EMPTY_COUNT_SEM_NUM {0};
constexpr short FILL_COUNT_SEM_NUM {1};
constexpr short BUFFER_RW_SEM_NUM {2};
constexpr short END_SUM_NUM {3};

int semaphore_id {};

int main(int argc, char** argv) {
    std::cout << "Producer" << std::endl;

    auto [buffer_length_shmem_id, buffer_length_ptr] = allocate_shmem_segment<int>(BUFFER_LENGTH_SHMEM_KEY);
    *buffer_length_ptr = BUFFER_LENGTH;

    auto [circular_buffer_shmem_id, circular_buffer] = allocate_shmem_segment<char>(BUFFER_SHMEM_KEY, BUFFER_LENGTH);

    semaphore_id = semget(SEM_KEY, 4, 0662 | IPC_CREAT);
    set_simple_semaphore_value(semaphore_id, BUFFER_RW_SEM_NUM, 1);
    set_simple_semaphore_value(semaphore_id, FILL_COUNT_SEM_NUM, 0);
    set_simple_semaphore_value(semaphore_id, EMPTY_COUNT_SEM_NUM, BUFFER_LENGTH);
    set_simple_semaphore_value(semaphore_id, END_SUM_NUM, 0);

    std::fstream file {FILE_PATH, std::ios::in};

    char character;
    int char_index {};
    while (file.get(character)) {
        decrement_simple_semaphore(semaphore_id, EMPTY_COUNT_SEM_NUM);
        decrement_simple_semaphore(semaphore_id, BUFFER_RW_SEM_NUM);
        circular_buffer[char_index] = character;
        std::cout << character << std::flush;
        increment_simple_semaphore(semaphore_id, BUFFER_RW_SEM_NUM);
        increment_simple_semaphore(semaphore_id, FILL_COUNT_SEM_NUM);

        char_index = (char_index + 1) % BUFFER_LENGTH;
    }
    decrement_simple_semaphore(semaphore_id, EMPTY_COUNT_SEM_NUM);
    decrement_simple_semaphore(semaphore_id, BUFFER_RW_SEM_NUM);
    circular_buffer[char_index] = '\0';
    increment_simple_semaphore(semaphore_id, BUFFER_RW_SEM_NUM);
    increment_simple_semaphore(semaphore_id, FILL_COUNT_SEM_NUM);

    std::cout << std::endl;

    file.close();
    increment_simple_semaphore(semaphore_id, END_SUM_NUM);
    return 0;
}
