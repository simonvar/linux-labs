#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>

#define BLUE_TEXT_COLOR             "\x1b[34m"
#define YELLOW_TEXT_COLOR           "\x1b[33m"
#define WHITE_TEXT_COLOR            "\x1b[37m"

#define OUTPUT_FILE "out_1.log"
#define printf //
static void close_zipper(int signo);

static int __counter = 0;
static int pipedes[2];

int main(int argc, char* argv[])
{

    if (argc < 3)
    {
        printf("Ошибка. Программа должна принимать 2 аргумента\n");
    }

    signal(SIGUSR1, close_zipper);    
    __pid_t child_id_1;
    __pid_t child_id_2;
    pipe(pipedes);    
    FILE *fout;
    fout = fopen(OUTPUT_FILE, "w");
    if((child_id_1 = fork()) == 0) 
    {
        FILE *fin1 = fopen(argv[1], "r");
        char ch;
        while (!feof(fin1))
        {
            ch = fgetc(fin1);
            if (ch != EOF)
                write(pipedes[1], &ch, 1);
        }
        kill(getppid(), SIGUSR1);
        fclose(fin1);
        _exit(0);
    }
    else
    {
        if ((child_id_2 = fork()) == 0)
        {
            FILE *fin2 = fopen(argv[2], "r");
            char ch;
            while (!feof(fin2))
            {
                ch = fgetc(fin2);
                if (ch != EOF)
                    write(pipedes[1], &ch, 1);
            }
            fclose(fin2);
            kill(getppid(), SIGUSR1);
            _exit(0);
        }
    }

    char ch;
    int buff;
    while ((buff = read(pipedes[0], &ch, sizeof(ch))) > 0)
    {
        printf("%c", ch);
        fprintf(fout, "%c", ch);
    }
    printf("Darova\n");
    
    printf("\n");

    fclose(fout);
    close(pipedes[0]);

    return 0;
}

static void close_zipper(int signo)
{
    printf(YELLOW_TEXT_COLOR"close_zipper\n"WHITE_TEXT_COLOR);
    __counter++;
    if (__counter == 2)
    {
        printf("Poka\n");
        close(pipedes[1]);
    }
}