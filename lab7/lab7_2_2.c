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

static FILE *fout2;
static int fildes[2];

static void test_child(int signo);
static void child_second_exit(int signo);

int main(int argc, char *argv[])
{
    fildes[0] = (int)argv[1][0];
    fildes[1] = (int)argv[1][1];
    printf("CHILD %d %d\n", fildes[0], fildes[1]);
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

static void test_child(int signo)
{
    char ch;
    int status = read(fildes[0], &ch, sizeof(ch));
    FILE *output_file;
    int out_signo;
    const char *text_color;
    int number_child;

    output_file = fout2;
    out_signo = SIGUSR1;
    text_color = BLUE_TEXT_COLOR;
    number_child = 2;

    
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

static void child_second_exit(int signo)
{
    printf("kill second\n");
    fclose(fout2);
    _exit(0);
}
