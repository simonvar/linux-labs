#include <stdio.h>
#include <pthread.h>

void* thread_func(void *args) {
    int fd = *((int*)(&args));
    FILE* file = fdopen(fd, "r");
    char str[255];
    while (fgets(str, sizeof(str), file) != NULL) {
	printf(str);
    }
    int fclose_result = fclose(file);
    return 0;
}

int main(int argc, char* argv[]) {
    pthread_t thread;
    int status;
    FILE *file = fopen(argv[1], "r");
    int fd = fileno(file);
    int thread_creating_result = pthread_create(&thread, NULL, thread_func, (void*)fd);
    int join_result = pthread_join(thread, (void**)&status);
    int file_close_result = fclose(file);
    return 0;
}

