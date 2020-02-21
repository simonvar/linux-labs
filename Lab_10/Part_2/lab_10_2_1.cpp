#include <iostream>
#include <string>
#include <fstream>

#include <unistd.h>
#include <sys/sem.h>

#include "sem_helpers.h"

constexpr key_t SEM_KEY {0xA0};
constexpr short FILE_RW_SEM_NUM {0};
constexpr short WRITERS_SEM_NUM {1};
constexpr short READERS_SEM_NUM {2};

const std::string FILE_NAME {"file_part_2.txt"};

int program_number {};
int number_of_lines_to_write {};
int time_interval {};


// Writer
int main(int argc, char** argv) {

    program_number = atoi(argv[1]);

    time_interval = atoi(argv[2]);

    number_of_lines_to_write = atoi(argv[3]);

    int semaphore_id = semget(SEM_KEY, 3, 0662 | IPC_CREAT);

    if (program_number == 1) {
        set_simple_semaphore_value(semaphore_id, FILE_RW_SEM_NUM, 1);
        set_simple_semaphore_value(semaphore_id, WRITERS_SEM_NUM, 0);
        set_simple_semaphore_value(semaphore_id, READERS_SEM_NUM, 0);
    }

    for (int i = 0; i < number_of_lines_to_write; i++) {
        sleep(time_interval);

        increment_simple_semaphore(semaphore_id, WRITERS_SEM_NUM);

        wait_fo_zero(semaphore_id, READERS_SEM_NUM);
        decrement_simple_semaphore(semaphore_id, FILE_RW_SEM_NUM);

        std::fstream file_to_write_into {FILE_NAME, std::fstream::app};
        std::cout << "Writer number " << program_number << " Line " << i << std::endl;
        file_to_write_into << "Writer number " << program_number << " Line " << i << std::endl;
        file_to_write_into.close();

        increment_simple_semaphore(semaphore_id, FILE_RW_SEM_NUM);
        decrement_simple_semaphore(semaphore_id, WRITERS_SEM_NUM);
    }

    // Create file sem
    // Create writers queue sem 
    // While writing increment both
    // After writing decrement both

    return 0;
}