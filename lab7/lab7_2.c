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

static void parent_sigusr1(int);
static void parent_sigusr2(int);
static void parent_sigterm(int);
static void test_child(int);
static void child_first_exit(int);
static void child_second_exit(int);
static void child1_sigusr1(int signo);
static void child1_sigusr2(int signo);

int main(int argc, char *argv[])
{

    if (argc < 2)
	{
        printf("Ошибка. Программа должна принимать один аргумента: имя входного файла\n");
        return 1;
    }

    pipe(fildes);
    fcntl(fildes[0], F_SETFL, O_NONBLOCK);
    printf("%d\n", getpid());
    signal(SIGUSR1, parent_sigusr1);
    signal(SIGUSR2, parent_sigusr2);

    pid_child1 = fork();
    if (!pid_child1)
    {
        // printf(YELLOW_TEXT_COLOR BOLD_FONT "Сhild 1 started"WHITE_TEXT_COLOR NORMAL_FONT"\n");
        signal(SIGUSR1, test_child);
        signal(SIGTERM, child_first_exit);
        fout1 = fopen(FILE_NAME_OUTPUT_CHILD_1, "w");
        while(true);
    }

    pid_child2 = fork();
    if (!pid_child2)
    {
        // printf(BLUE_TEXT_COLOR BOLD_FONT "Child 2 started"WHITE_TEXT_COLOR NORMAL_FONT"\n");
        signal(SIGUSR2, test_child);
        signal(SIGTERM, child_second_exit);
        fout2 = fopen(FILE_NAME_OUTPUT_CHILD_2, "w");
        while(true);
    }

    signal(SIGTERM, parent_sigterm);
    FILE* fin = fopen(argv[1], "r");
    char str[255] = "";
    while (fgets(str, sizeof(str), fin) != NULL)
    {
        write(fildes[1], str, sizeof(str));
    }
    fclose(fin);

    sleep(2);
    kill(-getpid(), SIGUSR1);
    int status;
    waitpid(pid_child1, &status, 0);
    waitpid(pid_child2, &status, 0);
    close(fildes[0]);
    close(fildes[1]);
    return 0;
}

static void parent_sigusr1(int signo)
{

}

static void parent_sigusr2(int signo)
{

}

static void parent_sigterm(int signo)
{
    kill(-getpid(), SIGTERM);
}

static void test_child(int signo)
{
    char ch;
    int status = read(fildes[0], &ch, sizeof(ch));
    FILE *output_file;
    int out_signo;
    const char *text_color;
    int number_child;
    if (signo == SIGUSR1)
    {
        output_file = fout1;
        out_signo = SIGUSR2;
        text_color = YELLOW_TEXT_COLOR;
        number_child = 1;
    }
    else
    {
        output_file = fout2;
        out_signo = SIGUSR1;
        text_color = BLUE_TEXT_COLOR;
        number_child = 2;
    }
    
    if (status == -1)
    {
        kill(-getppid(), SIGTERM);
    }
    if (status != 0)
    {
        // printf("%sCHILD %d: %c%s\n", text_color, number_child, ch, WHITE_TEXT_COLOR);
        fprintf(output_file, "%c", ch);
    }
    kill(-getppid(), out_signo);
}

static void child_first_exit(int signo)
{
    fclose(fout1);
    _exit(0);
}

static void child_second_exit(int signo)
{
    fclose(fout2);
    _exit(0);
}
