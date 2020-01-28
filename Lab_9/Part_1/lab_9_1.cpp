#include <unistd.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <errno.h>
#include <csignal>
// For start ./lab_9_1 0 1 5 & ./lab_9_1 1 2 4 & ./lab_9_1 2 3 3
#include <fstream>
#include <iostream>
#include <string>
#include <queue>

void timer_handler(int);

static const int NUMBER_OF_PROGRAMS {3};

bool* processes_ready_flags;
int* whose_turn;

int time_interval {};
int number_of_lines_to_write {};
int current_line_number {};
int program_number {};
bool should_run {true};

int main(int argc, char** argv) {
    program_number = atoi(argv[1]);
    time_interval = atoi(argv[2]);
    number_of_lines_to_write = atoi(argv[3]);
    {
        int mem_seg_id {shmget(0x79, NUMBER_OF_PROGRAMS*sizeof(bool), 0662 | IPC_CREAT)};
        if (mem_seg_id == -1) {
            std::cerr << "Error acquiring shared memory segment 0x79. Errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }

        processes_ready_flags = static_cast<bool*>(shmat(mem_seg_id, 0, 0));
    }

    {
        int mem_seg_id {shmget(0x80, sizeof(int), 0662 | IPC_CREAT)};
        if (mem_seg_id == -1) {
            std::cerr << "Error acquiring shared memory segment 0x80. Errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }

        whose_turn = static_cast<int*>(shmat(mem_seg_id, 0, 0));
    }

    itimerval value {
        {time_interval, 0},
        {time_interval, 0}
    };
    itimerval ovalue {};
    signal(SIGALRM, &timer_handler);
    setitimer(ITIMER_REAL, &value, &ovalue);

    while (should_run) {
        pause();
    }

    shmdt(processes_ready_flags);
    shmdt(whose_turn);

    return 0;
}

void timer_handler(int) {
    if (number_of_lines_to_write > 0) {
        processes_ready_flags[program_number] = true;
        

        int another_program_number_1 = (program_number + 1) % NUMBER_OF_PROGRAMS;
        int another_program_number_2 = (program_number + 2) % NUMBER_OF_PROGRAMS;

        // Waiting for other programs to unlock
        while (*whose_turn != program_number && 
                (processes_ready_flags[another_program_number_1] == true 
                || processes_ready_flags[another_program_number_2] == true));
                
        *whose_turn = program_number;
        // CRITICAL SECTION START
        number_of_lines_to_write--;
        current_line_number++;
        std::fstream file {"file.txt", std::fstream::app};

        file << "Program #" << program_number << ". Line №" << current_line_number << std::endl;
        std::cout << "Program #" << program_number << ". Line №" << current_line_number << std::endl;

        file.close();
        // CRITICAL SECTION END

        processes_ready_flags[program_number] = false;
    } else {
        setitimer(ITIMER_REAL, nullptr, nullptr);
        should_run = false;
    }
}