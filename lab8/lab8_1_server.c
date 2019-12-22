#include <errno.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>



typedef struct message {
    long type;
    int new_timer_interval;
} message_t;

#define true 1
#define false 0

// void timer_handler(int);
void create_timer(int timeout);

int messagequeue_id;

int main(int argc, char* argv[])
{
    int timeout;

    timeout = atoi(argv[1]);
    printf("Input timeout (seconds): %d\n", timeout);

    static const key_t queue_key = 121;
    int queue_id = msgget(queue_key, 0662 | IPC_CREAT);
    messagequeue_id = queue_id;
    if (queue_id == -1)
    {
        printf("Failed to create queue!\n");
        printf("Error code: %d\n", errno);
        _exit(1);
    }

    printf("Queue Id: %d\n", queue_id);
    printf("Waiting for message...\n");

    while (true)
    {
        create_timer(timeout);

        message_t received_message;
        long message_type = 42;
        
        int receiving_result = msgrcv(queue_id, &received_message, sizeof(message_t), message_type, 0);

        if (receiving_result == -1) 
        {
            printf("Error reding message. Error code: %d\n", errno);
            exit(1);
        }

        printf("Message received:\n");
        timeout = received_message.new_timer_interval;
        printf("New interval: %d s\n", timeout);
    }

    return 0;
}

void timer_handler(int value) 
{
    printf("Time up! Exiting...\n");
    int msqid = messagequeue_id;
    printf("Queue to close: %d\n", msqid);
    int operation_result = msgctl(msqid, IPC_RMID, NULL);
    if (operation_result == -1) {
        printf("Error deleting queue. Error code: %d\n", errno); 
    }
    exit(1);
}

void create_timer(int timeout) {
    setitimer(ITIMER_REAL, NULL, NULL);
    struct itimerval value;
    value.it_interval.tv_sec = timeout;
    value.it_value.tv_sec = timeout;

    struct itimerval ovalue;
    signal(SIGALRM, &timer_handler);
    alarm(timeout);
    // setitimer(ITIMER_REAL, &value, &ovalue);
}
