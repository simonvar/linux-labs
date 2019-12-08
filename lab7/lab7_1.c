#include <stdio.h>

#define __USE_GNU
#include <unistd.h>

#include <sys/wait.h>
#include <fcntl.h>

#define OUTPUT_FILE "out_1.log"

int main(int argc, char* argv[])
{

    if (argc < 2)
    {
        printf("Ошибка. Программа должна принимать 2 аргумента\n");
    }

    int pipedes[2];
    __pid_t child_id_1;
    __pid_t child_id_2;
    pipe2(pipedes, O_NONBLOCK);
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
            _exit(0);
        }
    }

    // fcntl(pipedes[0], F_SETFL, O_NONBLOCK);
    char ch;
    int buff;

    waitpid(child_id_1, NULL, 0);
    waitpid(child_id_2, NULL, 0);

    while ((buff = read(pipedes[0], &ch, sizeof(ch))) > 0)
    {
        printf("%c", ch);
        fprintf(fout, "%c", ch);
    }

    printf("\n");

    int status;
    wait(&status);
    wait(&status);
    fclose(fout);
    close(pipedes[0]);
    close(pipedes[1]);
    return 0;
}
