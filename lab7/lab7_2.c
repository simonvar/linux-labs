#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

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
static FILE *fout1, *fout2;

static void parentSIGUSR1(int);
static void parentSIGUSR2(int);
static void parentSIGTERM(int);
static void testChild(int);
static void childFirstExit(int);
static void childSecondExit(int);

int main(int argc, char *argv[])
{

    if (argc < 2)
	{
        printf("Ошибка. Программа должна принимать один аргумента: имя входного файла\n");
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
        signal(SIGUSR1, testChild);
        signal(SIGTERM, childFirstExit);
        fout1 = fopen(FILE_NAME_OUTPUT_CHILD_1, "w");
        while(true);
    }

    pid_child2 = fork();
    if (!pid_child2)
    {
        printf("\x1b[34m\x1b[1mChild 2 started\x1b[37;0m\n");
        signal(SIGUSR2, testChild);
        signal(SIGTERM, childSecondExit);
        fout2 = fopen(FILE_NAME_OUTPUT_CHILD_2, "w");
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
    waitpid(pid_child1, &status, 0);
    waitpid(pid_child2, &status, 0);

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

static void testChild(int signo)
{
    char ch;
    int status = read(fildes[0], &ch, sizeof(ch));
    FILE *output_file;
    int out_signo;
    const char *text_color;
    if (signo == SIGUSR1)
    {
        output_file = fout1;
        out_signo = SIGUSR2;
        text_color = BLUE_TEXT_COLOR;
    }
    else
    {
        output_file = fout2;
        out_signo = SIGUSR1;
        text_color = YELLOW_TEXT_COLOR;
    }
    
    if (status == -1)
    {
        kill(getppid(), SIGTERM);
    }
    if (status != 0)
    {
        printf("%sCHILD 1: %c%s\n", text_color, ch, WHITE_TEXT_COLOR);
        fprintf(output_file, "%c", ch);
    }
    kill(getppid(), out_signo);
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