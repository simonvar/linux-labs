#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/io.h>
#include <unistd.h>
#include <errno.h>

void aboutScheduler();

void* thread_func(void *args) {
    aboutScheduler();
    int fd = *((int*)(&args));
    char str[255];
    // while (read(fd, str, 255)) {
    // 	write(1, str, 255);
    // }
    read(fd, str, 255);
    write(1, str, 255);
    
    int fclose_result = close(fd);
    printf("close : %d\n", fclose_result);
    return 0;
}

int main(int argc, char* argv[]) {
    pthread_t thread;
    int res;
    int status;
    int fd = open(argv[1], O_RDONLY);
    int thread_creating_result = pthread_create(&thread, NULL, thread_func, (void*)fd);
    int join_result = pthread_join(thread, (void**)&status);

    // char str[255];
    // read(fd, str, 255);
    // write(1, str, 255);
    // printf("\n");

    int file_close_result = 0xFF;
    // file_close_result = close(fd);
    res = fcntl(fd, F_GETFL);
    printf("Close %d file %d errno %d\n", res, file_close_result, errno);
    if (!(res == -1 && errno == EBADF)) {
        file_close_result = close(fd);
    }
    return 0;
}

void aboutScheduler()
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