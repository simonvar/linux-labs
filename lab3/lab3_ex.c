#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void write_process_info(const char* filename, const char* tag);

int main(int argc, char* argv[]) {
    write_process_info(argv[1], "EXECV");
    return 0;
}

void write_process_info(const char* filename, const char* tag) {
    FILE* fp = fopen(filename, "a+");
    fprintf(fp, "%s: pid - %d\n", tag, getpid());
    fprintf(fp, "%s: parent pid - %d\n", tag, getppid());
    fprintf(fp, "%s: session id - %d\n", tag, getsid(getpid()));
    fprintf(fp, "%s: group id - %d\n", tag, getpgid(getpid()));
    fprintf(fp, "%s: real user id - %d\n", tag, getuid());
    fprintf(fp, "%s: effective user id - %d\n", tag, geteuid());
    fprintf(fp, "%s: group id - %d\n", tag, getgid());
    fprintf(fp, "%s: effective group id - %d\n", tag, getegid());
    fprintf(fp, "\n");
    fclose(fp);
}
