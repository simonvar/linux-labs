#include <stdio.h>
#include <time.h>

static void output();

int main(int argc, char *argv[])
{

    printf("Hello! ");
    output();
    return 0;
}

static void output()
{
    char buffer[255];
    time_t seconds = time(NULL);
    struct tm* timeinfo = localtime(&seconds);
    char* format = "%d.%B.%Y %H:%M:%S";
    strftime(buffer, sizeof(buffer), format, timeinfo);
    printf("TIME: %s\n", buffer);
}