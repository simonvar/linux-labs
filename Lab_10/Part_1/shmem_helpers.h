#pragma once

#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>

template<typename T = void>
std::pair<int, T*> allocate_shmem_segment(key_t shmem_key, unsigned segment_length = 1) {
    int mem_seg_id {shmget(shmem_key, segment_length * sizeof(T), 0662 | IPC_CREAT)};
    if (mem_seg_id == -1) {
        std::cerr << "Error acquiring shared memory segment with key " << shmem_key << ". Errno: " << errno << std::endl;
        return {};
    } else {
        std::cout << "Allocated segment with id " << mem_seg_id << std::endl;
        return {mem_seg_id, static_cast<T*>(shmat(mem_seg_id, 0, 0))};
    }
}

template<typename T = void>
std::pair<int, T*> get_shmem_segment(key_t shmem_key, unsigned segment_length = 1) {
    int mem_seg_id {shmget(shmem_key, segment_length * sizeof(T), 0662)};
    if (mem_seg_id == -1 && errno == ENOENT) {
        std::cerr << "Shared memory segment with key " << shmem_key << " does not exist. Errno: " << errno << std::endl;
        return {};
    } else if (mem_seg_id == -1) {
        std::cerr << "Error acquiring shared memory segment with key " << shmem_key << ". Errno: " << errno << std::endl;
        return {};
    } else {
        std::cout << "Grabbed segment with id " << mem_seg_id << std::endl;
        return {mem_seg_id, static_cast<T*>(shmat(mem_seg_id, 0, 0))};
    }
}