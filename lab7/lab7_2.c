#define __USE_POSIX
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#define true 1
#define false 0
#define FILE_NAME_OUTPUT_CHILD_1    "child_1.log"
#define FILE_NAME_OUTPUT_CHILD_2    "child_2.log"

#define BLUE_TEXT_COLOR             "\x1b[34m"
#define YELLOW_TEXT_COLOR           "\x1b[33m"
#define WHITE_TEXT_COLOR            "\x1b[37m"

#define BOLD_FONT                   "\x1b[1m"
#define NORMAL_FONT                 "\x1b[0m"
// TODO: Сделать возможность прописывать аргументом куда положить файл

static pid_t  pid_child1, pid_child2;
static int fildes[2];
// static FILE *fout1, *fout2;
static int ready_first_fl = false;
static int ready_second_fl = false;
static void parent_sigterm(int);
static void test_child(int);
static void child_first_exit(int);
static void child_second_exit(int);
static void child1_sigusr1(int signo);
static void child1_sigusr2(int signo);
static void ready_first(int signo);
static void ready_second(int signo);


int main(int argc, char *argv[])
{

    if (argc < 2)
	{
        printf("Ошибка. Программа должна принимать один аргумента: имя входного файла\n");
        return 1;
    }

    pipe(fildes);
    // fcntl(fildes[0], F_SETFL, 0);

    char *argv_for_child[] = {"/home/paladirka/Документы/linux-labs/lab7/lab7_2_1.out", "NULL", "NULL", NULL};
    argv_for_child[1] = malloc(sizeof(int)*2);
    argv_for_child[1][0] = fildes[0];
    argv_for_child[1][1] = fildes[1];

    printf("Parent %d %d\n", fildes[0], fildes[1]);

    pid_child1 = fork();
    if (!pid_child1)
    {
        int res = execv("lab7_2_1.out", argv_for_child);
        printf("res %d errno %d\n\n", res, errno);
        printf("Невозможно запустить процесс lab7_2_1.out %d\n", res);
		_exit(1);
    }

    pid_child2 = fork();
    if (!pid_child2)
    {
        int res = execv("lab7_2_2.out", argv_for_child);
        printf("res %d errno %d\n\n", res, errno);
        printf("Невозможно запустить процесс lab7_2_2.out %d\n", res);
		_exit(1);
    }

    struct sigaction sig1;
    struct sigaction sig2;
    sig1.sa_handler = ready_first;
    sig2.sa_handler = ready_second;

    sigaction(SIGUSR1, &sig1, NULL);
    sigaction(SIGUSR2, &sig2, NULL);
    printf("witing...");
    while(ready_first_fl == false || ready_second_fl == false)
    {
        pause();
    }
    printf("Ready all\n");
    signal(SIGTERM, parent_sigterm);
    kill(-getpid(), SIGUSR1);
    FILE* fin = fopen(argv[1], "r");
    char str[255] = "";
    while (fgets(str, sizeof(str), fin) != NULL)
    {
        write(fildes[1], str, sizeof(str));
    }
    fclose(fin);

    // sleep(2);
    int status;
    waitpid(pid_child1, &status, 0);
    waitpid(pid_child2, &status, 0);
    close(fildes[0]);
    close(fildes[1]);
    return 0;
}

static void ready_first(int signo)
{
    signal(signo, SIG_IGN);
    printf("First: Ready! %d\n", signo);
    ready_first_fl = true;
}

static void ready_second(int signo)
{
    signal(signo, SIG_IGN);
    printf("Second: Ready! %d\n", signo);
    ready_second_fl = true;
}

static void parent_sigterm(int signo)
{
    kill(pid_child1, SIGTERM);
    kill(pid_child2, SIGTERM);
}
