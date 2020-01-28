#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BLUE_TEXT_COLOR             "\x1b[34m"
#define YELLOW_TEXT_COLOR           "\x1b[33m"
#define WHITE_TEXT_COLOR            "\x1b[37m"

void about_scheduler();

void* thread_func(void *args) {
    about_scheduler();
    int fd = *((int*)(&args));
    char str[10] = {0};
    ssize_t reading;
    while (reading = read(fd, str, 10)) {
    	write(1, str, reading);
        // printf(BLUE_TEXT_COLOR"|fd = %d|"WHITE_TEXT_COLOR, fd);
    }
    
    // int fclose_result = close(fd);
    // printf("\nclose : %d (file %d)\n", fclose_result, fd);
    pthread_exit(0);
    // return 0;
}

int main(int argc, char* argv[]) {
    pthread_t thread;
    // int res;
    int status;
    int fd = open(argv[1], O_RDONLY);
    int fd_copy;
    printf("fd = %d; fd_copy = %d\n", fd, fd_copy);
    memcpy(&fd_copy, &fd, sizeof(int));
    printf("fd = %d; fd_copy = %d\n", fd, fd_copy);
    int thread_creating_result = pthread_create(&thread, NULL, thread_func, (void*)fd);
    printf("fd = %d; fd_copy = %d\n", fd, fd_copy);
    int join_result = pthread_join(thread, (void**)&status);
    printf("fd = %d; fd_copy = %d\n", fd, fd_copy);
    __off64_t res = lseek(fd_copy, 0, SEEK_SET);
    printf("fd = %d; fd_copy = %d\n", fd, fd_copy);
    // printf("Close %ld file errno %d\n\n", res, errno);
    if ((errno != EBADF) && (res != -1)) {
        int file_close_result = close(fd_copy);
        printf("Closeing file %d\n", file_close_result);
    }
    return 0;
}

void about_scheduler()
{	
	int scheduler = sched_getscheduler(getpid());	
	struct sched_param p;
    switch(scheduler)
	{
    case SCHED_FIFO:	
        printf("\nSCHED_FIFO (0) - политика планирования реального времени первый вошел, первый вышел.\n");
        break; 
    case SCHED_RR:
        printf("\nSCHED_RR (1) - циклическая политика планирования реального времени.\n");
        break;
    case SCHED_OTHER:
        printf("\nSCHED_OTHER (2) - политика планирования с разделением времени, время выполнения определяется динамическим приоритетом.\n");
        break;
	}
	sched_getparam(0, &p);
    printf("\nТекущий приоритет: %d\n",  p.sched_priority);
    printf("Максимальное разрешенное значение статического приоритета: %d \n", sched_get_priority_max(scheduler));
    printf("Минимальное разрешенное значение статического приоритета: %d \n\n\n",  sched_get_priority_min(scheduler));
}