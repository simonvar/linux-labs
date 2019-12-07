#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <fcntl.h>


int main(int argc, char* argv[])
{

    if (argc < 3)
    {
        printf("Ошибка. Программа должна принимать 3 аргумента\n");
    }

    int fildes[2];
    pipe(fildes);
    FILE *fout;
    fout = fopen(argv[3], "w");
    if(fork() == 0) 
    {
        FILE *fin1 = fopen(argv[1], "r");
        char ch;
        while (!feof(fin1))
        {
            ch = fgetc(fin1);
            if (ch != EOF)
                write(fildes[1], &ch, 1);
        }

        fclose(fin1);
    }
    else
    {
        if (fork() == 0)
        {
            FILE *fin2 = fopen(argv[2], "r");
            char ch;
            while (!feof(fin2))
            {
                ch = fgetc(fin2);
                if (ch != EOF)

                write(fildes[1], &ch, 1);
            }
            fclose(fin2);
        }

    }

    fcntl(fildes[0], F_SETFL, O_NONBLOCK);
    char ch;
    int buff;
    while ((buff = read(fildes[0], &ch, sizeof(ch))) > 0)
    {
        printf("%c", ch);
        fprintf(fout, "%c", ch);
    }

    int status;
    wait(&status);
    wait(&status);
    fclose(fout);
    close(fildes[0]);
    close(fildes[1]);
    return 0;
}
