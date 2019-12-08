#include <stdio.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "massage.h"


void stop_client();

int main(int argc, char* argv[])
{
    int id_server = msgget(IPC_SET, IPC_EXCL);
    if (id_server == -1)
    {
        stop_client();
    }

    id_server = msgget(1, IPC_CREAT);
    if (id_server == -1)
    {
        msgctl(id_server, IPC_RMID, NULL);
        stop_client();
    }

    struct MESSAGE message;
    strcpy(message.msg, "test message");
    message.timeout = atoi(argv[1]);

    msgsnd(id_server, &message, sizeof(struct MESSAGE), 0);
    printf("Отправлено сообщение: %s| timeout: %d sec\n", message.msg, message.timeout);
    return 0;
}

static void stop_client()
{
    printf("Ошибка. Не удалось установить связь с сервером\n");
    raise(SIGKILL);
}
