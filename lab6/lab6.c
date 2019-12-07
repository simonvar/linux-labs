#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define bool int
#define false 0
#define true 1

static int iterations = 0;
static int count = 0;

void handler(int);
void output(bool);
void stopProgram(int);

// TODO: сделать бесконечное число итераций
// TODO: возможность остановки таймера (параметром)

int main(int argc, char* argv[])
{
    if (argc < 3) 
	{
        printf("Ошибка. Программа должна принимать два аргумента\n");
        return 1;
    }

    iterations = atoi((const char*)argv[1]);
    int interval = atoi((const char*)argv[2]);

    printf("Iterations: %d | Interval: %d\n", iterations, interval);

    signal(SIGTSTP, stopProgram);
    signal(SIGALRM, handler);


    struct itimerval timer;
    timer.it_value.tv_sec = interval;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    while (1);
}

void handler(int signo)
{
    pid_t childPid = fork();
    if(!childPid)
	{
        output(false);
        return;
    }
    waitpid(childPid, NULL, 0);
    output(true);
    count++;
    if(count >= iterations)
        _exit(0);
}

void output(bool isParent)
{
    char buffer[255];
    time_t seconds = time(NULL);
    struct tm* timeinfo = localtime(&seconds);
    char* format = "%d.%B.%Y %H:%M:%S";
    strftime(buffer, sizeof(buffer), format, timeinfo);
    printf("%s PID: %d TIME: %s\n", (isParent ? "Parent." : "Child."), getpid(), buffer);
    if (!isParent)
	{
        _exit(0);
	}
}

void stopProgram(int signum)
{
    printf("Подождите пока программа завершится\n");
}