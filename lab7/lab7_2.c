#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define true 1
#define false 0

static pid_t  pid_child1, pid_child2;
static int fildes[2];
static FILE *fout1, *fout2;

static void parentSIGUSR1(int);
static void parentSIGUSR2(int);
static void parentSIGTERM(int);
static void testChild1(int);
static void testChild2(int);
static void childFirstExit(int);
static void childSecondExit(int);

int main(int argc, char *argv[])
{

    if (argc < 3)
	{
        printf("Ошибка. Программа должна принимать два аргумента\n");
        return 1;
    }

    pipe(fildes);
    fcntl(fildes[0], F_SETFL, O_NONBLOCK);

    signal(SIGUSR1, parentSIGUSR1);
    signal(SIGUSR2, parentSIGUSR2);
    signal(SIGTERM, parentSIGTERM);

    pid_child1 = fork();
    if (!pid_child1)
    {
        printf("\x1b[33;1mСhild 1 started\x1b[37;0m\n");
        signal(SIGUSR1, testChild1);
        signal(SIGTERM, childFirstExit);
        fout1 = fopen(argv[2], "w");
        while(true);
    }

    pid_child2 = fork();
    if (!pid_child2)
    {
        printf("\x1b[34;1mChild 2 started\x1b[37;0m\n");
        signal(SIGUSR2, testChild2);
        signal(SIGTERM, childSecondExit);
        fout2 = fopen(argv[3], "w");
        while(true);
    }

    FILE* fin = fopen(argv[1], "r");
    char str[255] = "";
    while (fgets(str, sizeof(str), fin) != NULL)
    {
        write(fildes[1], str, sizeof(str));
        strcpy(str, "");
    }
    fclose(fin);

    sleep(2);
    parentSIGUSR1(0);
    int status;
    wait(&status);
    wait(&status);

    close(fildes[0]);
    close(fildes[1]);
    return 0;
}

static void parentSIGUSR1(int signo)
{
    kill(pid_child1, SIGUSR1);
}

static void parentSIGUSR2(int signo)
{
    kill(pid_child2, SIGUSR2);
}

static void parentSIGTERM(int signo)
{
    kill(pid_child1, SIGTERM);
    kill(pid_child2, SIGTERM);
}

static void testChild1(int signo)
{
    char ch;
    int status = read(fildes[0], &ch, sizeof(ch));
    if (status == -1)
    {
        kill(getppid(), SIGTERM);
    }
    if (ch != '\0')
    {
        printf("\x1b[33mCHILD 1: %c\x1b[37m\n", ch);
        fprintf(fout1, "%c", ch);
    }
    kill(getppid(), SIGUSR2);
}

static void testChild2(int signo)
{
    char ch;
    int status = read(fildes[0], &ch, sizeof(ch));
    if (status == -1)
    {
        kill(getppid(), SIGTERM);
    }
    if (ch != '\0')
    {
        printf("\x1b[34mCHILD 2: %c\x1b[37m\n", ch);
        fprintf(fout2, "%c", ch);
    }
    kill(getppid(), SIGUSR1);
}

static void childFirstExit(int signo)
{
    fclose(fout1);
    _exit(0);
}

static void childSecondExit(int signo)
{
    fclose(fout2);
    _exit(0);
}