#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "massage.h"

#define bool        int
#define true        1
#define false       0


static int id_server;

static void end_server(int);
static void stop_server_by_timer(int);

int main(int argc, char* argv[])
{
    struct MESSAGE message;

    id_server = msgget(IPC_SET, 0600 | IPC_CREAT); // Есть ощущение что 0600 - O_EXCL | O_NOCTTY - но почему они не понятно
    if (id_server == -1)
    {
        printf("Ошибка. Не удалось создать очередь\n");
        return 1;
    }

    signal(SIGINT, end_server);
    signal(SIGALRM, stop_server_by_timer);

    struct itimerval tval;
    tval.it_interval.tv_sec = atoi(argv[1]);
    tval.it_interval.tv_usec = 0;
    tval.it_value.tv_sec = atoi(argv[1]);
    tval.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &tval, NULL);

    bool messageReceive = false;
    while(1)
    {
        int status = msgrcv(id_server, &message, sizeof(message), 0, IPC_NOWAIT);
        if (status > 0)
        {
            if (!messageReceive)
            {
                tval.it_interval.tv_sec = 0;
                tval.it_value.tv_sec = 0;
                setitimer(ITIMER_REAL, &tval, NULL);
                messageReceive = true;
            }
            printf("Получено сообщение: %s | таймаут: %d sec\n", message.msg, message.timeout);
            sleep(message.timeout);
        }
        else if (status == -1 && messageReceive)
        {
            end_server(0);
        }
    }
}

static void end_server(int signo)
{
    msgctl(id_server, IPC_RMID, NULL);
    raise(SIGKILL);
}

static void stop_server_by_timer(int signo)
{
    printf("Программа завершена по таймеру\n");
    msgctl(id_server, IPC_RMID, NULL);
    raise(SIGKILL);
}