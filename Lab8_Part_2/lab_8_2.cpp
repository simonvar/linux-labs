#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <map>
#include <queue>
#include <ctime>

#include <unistd.h>
#include <sys/msg.h>


using namespace std;

struct read_request_msg_t {
    long type;
    int response_queue_id;
    int receiver_program_number;
    int sender_program_number;
    time_t request_time;

    bool operator == (const read_request_msg_t& rhs) {
        return type == rhs.type
            && response_queue_id == rhs.response_queue_id
            && receiver_program_number == rhs.receiver_program_number
            && sender_program_number == rhs.sender_program_number
            && request_time == rhs.request_time;
    }
};

struct read_approval_msg_t {
    long type;
    int sender_program_number;
    time_t response_time;
};

static const string FILE_PATH {"Example.txt"};
static const int GLOBAL_QUEUE_KEY = 121;
static const int LOCAL_QUEUE_MSG_TYPE = 10;

int this_program_number {-1};
int global_queue_id {-1};
int local_queue_id {-1};
std::map<int, time_t> times_of_requests_to_programs {};
bool file_reading_is_permitted_by_the_first_program {false};
bool file_reading_is_permitted_by_the_second_program {false};

int number_of_sent_responses {};

void parse_args(int argc, char** argv);
void ask_program(int program_to_ask_number);
void try_to_read_file();
void manage_request(const read_request_msg_t& message);
bool read_file();
std::vector<read_request_msg_t> process_requests_queue(std::vector<read_request_msg_t>&& messages_to_manage, bool file_was_read);

int main(int argc, char** argv) {
    parse_args(argc, argv);

    global_queue_id = msgget(GLOBAL_QUEUE_KEY, 0662 | IPC_CREAT);
    local_queue_id = msgget(IPC_PRIVATE, 0662 | IPC_CREAT);

    int first_prog_number = (this_program_number + 1) % 3;
    ask_program(first_prog_number);
    cout << "Asking program with number: " << first_prog_number << endl;
    int second_prog_number = (this_program_number + 2) % 3;
    ask_program(second_prog_number);
    cout << "Asking program with number: " << second_prog_number << endl;

    try_to_read_file();

    return 0;
}

void parse_args(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Please provide '-n' arg with program number from 0 to 2." << endl;
        exit(EXIT_FAILURE);
    }

    int opt {};
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        if (opt == 'n') {
            try {
                this_program_number = std::stoi(optarg);
                if (this_program_number < 0 || this_program_number > 2) {
                    cerr << "Argument must be in range [0;2]" << endl;
                    exit(EXIT_FAILURE);
                }
                cout << "This program number: " << this_program_number << endl;
            } catch (const std::invalid_argument& e) {
                cerr << "Argument must be integer!" << endl;
                exit(EXIT_FAILURE);
            } catch (const std::out_of_range& e) {
                cerr << "Argument is out of range!" << endl;
                exit(EXIT_FAILURE);
            }
        } else {
            cerr << "No such option '" << (char)opt << "'" << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void ask_program(int program_to_ask_number) {
    auto now = std::chrono::system_clock::now();
    time_t current_time = std::chrono::system_clock::to_time_t(now);
    read_request_msg_t request_to_program {
        program_to_ask_number + 1,
        local_queue_id,
        program_to_ask_number,
        this_program_number,
        current_time
    };
    msgsnd(
        global_queue_id,
        &request_to_program,
        sizeof(read_request_msg_t) - sizeof(read_request_msg_t::type),
        IPC_NOWAIT
    );
    times_of_requests_to_programs.emplace(std::make_pair(program_to_ask_number, current_time));
}

void try_to_read_file() {
    bool file_was_read {false};
    std::vector<read_request_msg_t> messages_to_manage {};

    while (true) {
        if (!file_was_read && file_reading_is_permitted_by_the_first_program && file_reading_is_permitted_by_the_second_program) { 
            file_was_read = read_file();
            // cout << "File was read!" << endl;
        }

        read_request_msg_t received_request {};
        int request_receiving_result = msgrcv(
            global_queue_id,
            &received_request,
            sizeof(read_request_msg_t) - sizeof(read_request_msg_t::type),
            this_program_number + 1,
            IPC_NOWAIT
        );

        if (request_receiving_result != -1) {
            messages_to_manage.emplace_back(received_request);
            // cout << "Message from program with queue id " << received_request.response_queue_id << " received and placed in the queue!" << endl;
        }

        read_approval_msg_t received_approval {};
        int approval_receiving_result = msgrcv(
            local_queue_id,
            &received_approval,
            sizeof(read_approval_msg_t) - sizeof(read_approval_msg_t::type),
            LOCAL_QUEUE_MSG_TYPE,
            IPC_NOWAIT
        );

        if (approval_receiving_result != -1) {
            int first_prog_number = (this_program_number + 1) % 3;
            int second_prog_number = (this_program_number + 2) % 3;
            int approving_program_number = received_approval.sender_program_number;
            time_t time_of_response = received_approval.response_time;
            if (approving_program_number == first_prog_number) {
                file_reading_is_permitted_by_the_first_program = true;
                cout << "Approved by the program " << approving_program_number << " (the first one) with approval sent at " << ctime(&time_of_response);
            } else if (approving_program_number == second_prog_number) {
                file_reading_is_permitted_by_the_second_program = true;
                cout << "Approved by the program " << approving_program_number << " (the second one) with approval sent at " << ctime(&time_of_response);
            } else {
                cerr << "Approved by unknown program with number " << approving_program_number << endl;
            }
        }

        messages_to_manage = process_requests_queue(std::move(messages_to_manage), file_was_read);

        if (file_was_read && number_of_sent_responses >= 2) {
            int local_queue_deletion_result = msgctl(local_queue_id, IPC_RMID, nullptr);
            cout << "Exiting..." << endl;
            break;
        }

        usleep(100);
    }
}

void manage_request(const read_request_msg_t& message) {
    auto now = std::chrono::system_clock::now();
    time_t current_time = std::chrono::system_clock::to_time_t(now);
    read_approval_msg_t response {LOCAL_QUEUE_MSG_TYPE, this_program_number, current_time};
    int sending_result = msgsnd(
        message.response_queue_id,
        &response,
        sizeof(read_approval_msg_t) - sizeof(read_approval_msg_t::type),
        IPC_NOWAIT
    );
    number_of_sent_responses++;
}

bool read_file() {
    ifstream ifs {FILE_PATH};
    string file_line {};
    cout << "File contents: " << endl;
    while (std::getline(ifs, file_line)) {
        cout << "    " << file_line << endl;
    }
    
    return true;
}

std::vector<read_request_msg_t> process_requests_queue(std::vector<read_request_msg_t>&& messages_to_manage, bool file_was_read) {
    auto msg_queue = std::move(messages_to_manage);
    for (auto& processing_message : msg_queue) {
        time_t time_of_processing_message {processing_message.request_time};
        time_t time_of_sending_message_to_program_of_proccesing_message {times_of_requests_to_programs[processing_message.sender_program_number]};

        if (file_was_read || time_of_processing_message < time_of_sending_message_to_program_of_proccesing_message) {
            manage_request(processing_message);

            // cout << "Message from program with queue id " << processing_message.response_queue_id << " proccesed!" << endl;
            msg_queue.erase(std::remove(msg_queue.begin(), msg_queue.end(), processing_message), msg_queue.end());
        }
    }
    return msg_queue;
}