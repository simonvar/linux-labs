#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void handler_divide(int signum);
void handler_ptr(int signum);

int main(int argc, char* argv[]) {

    if (argc <= 1) {
        puts("Не задан тип ошибки (SIGFPE SIGSEGV)");
        return 0;
    }

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGFPE);
    sigaddset(&set, SIGSEGV);
    sigprocmask(SIG_UNBLOCK, &set, NULL);

    if (strcmp(argv[1], "SIGFPE") == 0) {
        signal(SIGFPE, handler_divide);
        int answer = 1/0;
        printf("RESULT: %d", answer);
    } else if (strcmp(argv[1], "SIGSEGV") == 0) {
        signal(SIGSEGV, handler_ptr);
        int* p = 0x00000000;
        *p = 10;
    } else {
        puts("Ошибка. Неверно введен тип ошибки");
    }

    return 0;
}

void handler_divide(int signum) {
    puts("Ошибка. Деление на ноль");
    _exit(3);
}

void handler_ptr(int signum) {
    puts("Ошибка. Нарушение защиты памяти");
    _exit(3);
}

