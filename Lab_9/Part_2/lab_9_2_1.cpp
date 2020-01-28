#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/shm.h>

static const key_t ARRAY_TO_DECREMENT_KEY = 131;
static const key_t ARRAY_TO_DECREMENT_SIZE_PTR_KEY = 132;
static const key_t DECREMENTED_ARRAY_KEY = 133;
static const key_t DECREMENTED_ARRAY_SIZE_PTR_KEY = 134;
static const key_t CAN_SEND_PTR_KEY = 135;

static const key_t FIRST_END = 136;
static const key_t SECOND_END = 137;


std::vector<int> shared_memory_segments_ids {};

int* array_to_decrement {};
int* array_to_decrement_size_ptr {};

int* decremented_array {};
int* decremented_array_size_ptr {};

int* can_send_ptr {};

int* first_end {};
int* second_end {};

int* allocate_int_segment_in_shared_memory(key_t shmem_key, size_t seg_size);
int* get_existing_int_segment_in_shared_memory(key_t shmem_key, size_t seg_size);
void release_shmem_segments();

int main(int argc, char** argv) {

    can_send_ptr = allocate_int_segment_in_shared_memory(CAN_SEND_PTR_KEY, sizeof(int));
    *can_send_ptr = 0;

    first_end = allocate_int_segment_in_shared_memory(FIRST_END, sizeof(int));
    *first_end = 0;
    second_end = allocate_int_segment_in_shared_memory(SECOND_END, sizeof(int));

    int array_size {};

    while (array_size <= 0) {
        std::cout << "Input array size (> 0): ";
        std::cin >> array_size;
    }

    std::cout << "Input an array of " << array_size << " elements: ";
    
    std::vector<int> input_array {};
    input_array.reserve(array_size);
    for (int i = 0; i < array_size; i++) {
        int number {};
        std::cin >> number;
        input_array.emplace_back(number);
    }

    array_to_decrement = allocate_int_segment_in_shared_memory(ARRAY_TO_DECREMENT_KEY, array_size*sizeof(int));
    std::copy(input_array.cbegin(), input_array.cend(), array_to_decrement);

    array_to_decrement_size_ptr = allocate_int_segment_in_shared_memory(ARRAY_TO_DECREMENT_SIZE_PTR_KEY, sizeof(int));
    *array_to_decrement_size_ptr = array_size;

    *can_send_ptr = 1;

    std::cout << "Waiting for the second program to decrement array..." << std::endl;
    while (*can_send_ptr == 1);

    decremented_array_size_ptr = get_existing_int_segment_in_shared_memory(DECREMENTED_ARRAY_SIZE_PTR_KEY, sizeof(int));

    decremented_array = get_existing_int_segment_in_shared_memory(DECREMENTED_ARRAY_KEY, *decremented_array_size_ptr*sizeof(int));

    std::cout << "Decremented array: ";
    for (int i = 0; i < *decremented_array_size_ptr; i++) {
        std::cout << decremented_array[i] << " ";
    }
    std::cout << std::endl;

    *first_end = 1;
    while(*second_end != 1);
    release_shmem_segments();
    return 0;
}

int* allocate_int_segment_in_shared_memory(key_t shmem_key, size_t seg_size) {
    int mem_seg_id {shmget(shmem_key, seg_size, 0666 | IPC_CREAT)};
    if (mem_seg_id == -1) {
        std::cerr << "Error acquiring shared memory segment with key " << shmem_key << ". Errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    } else {
        shared_memory_segments_ids.emplace_back(mem_seg_id);
        std::cout << "Allocated segment with id " << mem_seg_id << std::endl;
        return static_cast<int*>(shmat(mem_seg_id, nullptr, 0));
    }
}

int* get_existing_int_segment_in_shared_memory(key_t shmem_key, size_t seg_size) {
    int mem_seg_id {shmget(shmem_key, seg_size, 0666)};
    if (mem_seg_id == -1 && errno == ENOENT) {
        return nullptr;
    } else if (mem_seg_id == -1) {
        std::cerr << "Error acquiring shared memory segment with key " << shmem_key << ". Errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    } else {
        
        std::cout << "Grabbed segment with id " << mem_seg_id << std::endl;
        return static_cast<int*>(shmat(mem_seg_id, nullptr, 0));
    }
}

void release_shmem_segments() {
    for (int mem_seg_id : shared_memory_segments_ids) {
        shmctl(mem_seg_id, IPC_RMID, nullptr);
        std::cout << "Segment with id " << mem_seg_id << " removed!" << std::endl;
    }
}