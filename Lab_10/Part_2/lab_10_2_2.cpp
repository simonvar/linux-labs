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
int time_interval {};


// READER
int main(int argc, char** argv) {

    program_number = atoi(argv[1]);

    time_interval = atoi(argv[2]);
    

    int semaphore_id = semget(SEM_KEY, 3, 0662 | IPC_CREAT);
 

    std::streampos cours = 0;
    std::fstream file_to_read;

    do{
        sleep(time_interval);
        wait_fo_zero(semaphore_id, WRITERS_SEM_NUM);
        increment_simple_semaphore(semaphore_id, READERS_SEM_NUM);
        file_to_read.open(FILE_NAME);
        file_to_read.seekg(cours);
        std::string line {};
        getline(file_to_read, line);

        std::cout << line << std::endl;
        cours = file_to_read.tellg();
        file_to_read.close();
        decrement_simple_semaphore(semaphore_id, READERS_SEM_NUM);
    }while (!file_to_read.eof());
    // std::cout << "I READ LINE" << std::endl;



    

    // Connect to the two sems
    // Lock file sem
    // Check if writers sem is zero
    // Read file
    // Unlock file sem

    return 0;
}