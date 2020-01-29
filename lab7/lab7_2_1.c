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

static FILE *fout1;
static int fildes[2];

static void test_child(int signo);
static void child_first_exit(int signo);

int main(int argc, char *argv[])
{
    fildes[0] = (int)argv[1][0];
    fildes[1] = (int)argv[1][1];
    printf("CHILD %d %d\n", fildes[0], fildes[1]);
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

static void test_child(int signo)
{
    char ch;
    int status = read(fildes[0], &ch, sizeof(ch));
    FILE *output_file;
    int out_signo;
    const char *text_color;
    int number_child;

    output_file = fout1;
    out_signo = SIGUSR2;
    text_color = YELLOW_TEXT_COLOR;
    number_child = 1;

    
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
