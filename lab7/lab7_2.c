#define __USE_POSIX
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

    pid_child1 = fork();
    if (!pid_child1)
    {
        printf(YELLOW_TEXT_COLOR BOLD_FONT "Сhild 1 started"WHITE_TEXT_COLOR NORMAL_FONT"\n");
        struct sigaction sig_1_2;
        struct sigaction sig_2_2;
        struct sigaction sig_3_2;
        sig_1_2.sa_handler = test_child;
        sig_2_2.sa_handler = SIG_IGN;
        sig_3_2.sa_handler = child_first_exit;
        sigaction(SIGUSR1, &sig_1_2, NULL);
        sigaction(SIGUSR2, &sig_2_2, NULL);
        sigaction(SIGTERM, &sig_3_2, NULL);
        // signal(SIGUSR1, test_child);
        // signal(SIGUSR2, SIG_IGN);
        // signal(SIGTERM, child_first_exit);
        fout1 = fopen(FILE_NAME_OUTPUT_CHILD_1, "w");
        kill(getppid(), SIGUSR1);
        while(true) pause();
    }

    pid_child2 = fork();
    if (!pid_child2)
    {

        printf(BLUE_TEXT_COLOR BOLD_FONT "Child 2 started"WHITE_TEXT_COLOR NORMAL_FONT"\n");
        struct sigaction sig_1_1;
        struct sigaction sig_2_1;
        struct sigaction sig_3_1;
        sig_1_1.sa_handler = SIG_IGN;
        sig_2_1.sa_handler = test_child;
        sig_3_1.sa_handler = child_second_exit;
        sigaction(SIGUSR1, &sig_1_1, NULL);
        sigaction(SIGUSR2, &sig_2_1, NULL);
        sigaction(SIGTERM, &sig_3_1, NULL);

        // signal(SIGUSR1, SIG_IGN);
        // signal(SIGUSR2, test_child);
        // signal(SIGTERM, child_second_exit);
        fout2 = fopen(FILE_NAME_OUTPUT_CHILD_2, "w");
        kill(getppid(), SIGUSR2);
        while(true) pause();
    }

    struct sigaction sig1;
    struct sigaction sig2;
    sig1.sa_handler = ready_first;
    sig2.sa_handler = ready_second;

    sigaction(SIGUSR2, &sig1, NULL);
    sigaction(SIGUSR1, &sig2, NULL);
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
    
    if ((status == -1) || (ch == '\0'))
    {
        printf("kill SIGTERM\n");
        kill(getppid(), SIGTERM);
    }
    if (status != 0)
    {
        printf("%sCHILD %d: %c%s\n", text_color, number_child, ch, WHITE_TEXT_COLOR);
        fprintf(output_file, "%c", ch);
    }
    signal(signo, test_child);
    kill(-getppid(), out_signo);
}

static void child_first_exit(int signo)
{
    printf("kill first\n");
    fclose(fout1);
    _exit(0);
}

static void child_second_exit(int signo)
{
    printf("kill second\n");
    fclose(fout2);
    _exit(0);
}
