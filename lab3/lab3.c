#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void write_process_info(const char* filename, const char* tag);

int main(int argc, char* argv[]) {

	printf("Hello! I'm parent (%d)\n", getpid());
    if (argc < 5) {
        puts("Аргументы: путь к файлу, задержка для родительского процесса, задержка для fork, задержка для vfork.");
        return EINVAL;
    }

	// clear file
	FILE* fp = fopen(argv[1], "w");
	fclose(fp);
	pid_t pid = fork();
	printf("Hello! I'm %d\n", getpid());
	if (pid == 0) {
		sleep(atoi(argv[3]));
		write_process_info(argv[1], "CHILD 1");
		_exit(0);
	}

	if (vfork() == 0) {
    	sleep(atoi(argv[4]));
        execv("lab3_ex.out", argv);
		puts("Невозможно запустить процесс lab3_ex.out");
		_exit(ENOENT);
	}

	sleep(atoi(argv[2]));
    write_process_info(argv[1], "PARENT");

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
