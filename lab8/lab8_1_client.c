#include <stdio.h>

#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>


typedef struct message {
    long type;
    int new_timer_interval;
} message_t;

int main(int argc, char* argv[])
{
    static const key_t queue_key = 121;
    int queue_id = msgget(queue_key, 0662);

    if (queue_id == -1) {
        printf("Queue does not exist yet! Exiting...\n");
        exit(1);
    }

    printf("Queue Id: %d\n", queue_id);

    int new_timeout;
    new_timeout = atoi(argv[1]);
    printf("Input NEW timeout (seconds): %d\n", new_timeout);

    long message_type = 42;
    message_t message = 
    {
        .type = message_type, 
        .new_timer_interval = new_timeout
    };
    int operation_result = msgsnd(queue_id, &message, sizeof(message_t), 0);

    if (operation_result == -1) {
        printf("Error. Message not sent\n");
        printf("Errno: %d\n", errno);
    } else {
        printf("Message sent!\n");
    }

    return 0;
}
