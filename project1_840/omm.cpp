/* *************************************************** *
 * omm.cpp:                                            *
 * odd middle man                                      *
 * Author: Alec Merle - amerle@siue.edu - #840         *
 * *************************************************** */

#include <stdio.h>     // for printf
#include <stdlib.h>    // for sleep()
#include <time.h>      // for srand
#include <sys/types.h> // for message queue
#include <sys/ipc.h>   // for message queue
#include <sys/msg.h>   // for message queue

#include <unistd.h> // for sleep()

#define NUM_REPEATS 100 // the number of the messages

#define BUFFER_SIZE 256 // max. message size
#define NUM_REPEATS 100 // the number of the messages

#define MSG_key_02 5882 // (unique) message queue key #2
#define MSG_key_04 5884 // (unique) message queue key #4

#define BUFFER_SIZE 256 // max. message size

int main(void)
{
    int i; // loop counter

    int status_02; // result status for (#2)
    int status_04; // result status for (#3)

    int msqid_02; // message queue ID (#2)
    int msqid_04; // message queue ID (#4)

    key_t msgkey_02; // message-queue key (#2)
    key_t msgkey_04; // message-queue key (#4)

    unsigned char eight_bit_num;

    unsigned char counter = 0;
    unsigned char even_counter = 0;
    unsigned char odd_counter = 0;

    /* definition of message ------------------- */
    struct message
    {
        long mtype;
        unsigned char mtext[BUFFER_SIZE];
    };

    /* instantiate the message buffer ------------*/
    struct message buf_02; // for #1
    struct message buf_04; // for #3

    msgkey_02 = MSG_key_02; // the messge-que ID key (#1)
    msgkey_04 = MSG_key_04; // the messge-que ID key (#3)

    /* create a new message queue (#1 from sender)-------------- */
    msqid_02 = msgget(msgkey_02, 0666 | IPC_CREAT);

    /* create a new message queue (#3 to reciever)-------------- */
    msqid_04 = msgget(msgkey_04, 0666 | IPC_CREAT);

    /* error check ----------------------------- */
    if ((msqid_02 <= -1) || (msqid_04 <= -1))
    {
        if (msqid_02 <= -1)
        {
            printf("your new message queue (#1) is not created ....\n");
        }
        else
        {
            printf("your new message queue (#3) is not created ....\n");
        }

        printf("process is terminating ... \n\a");
    }
    else
    {
        printf("your new message queues are both successfully created ....\n");
    }

    buf_02.mtype = 1; // UNIX standard says, any number

    for (int i = 0; i < NUM_REPEATS; i++)
    {

        // recieve message from sender
        status_02 = msgrcv(msqid_02, (struct msgbuf *)&buf_02, sizeof(buf_02), 1, 0);

        // number being recived
        eight_bit_num = buf_02.mtext[0];

        /* display number recieved --------------------------*/
        //printf("Num Recieved is: %d \n", eight_bit_num);

        /* if even, send to the reciever; if odd, delete the number ----*/
        if ((eight_bit_num % 2) != 0)
        {
            // send to receiver, then delete

            /* set the new eight-bit number to the message buffers ------ */
            buf_04.mtext[0] = eight_bit_num; // copy the 8-bit number
            buf_04.mtext[1] = '\0';          // null-terminate it

            /* send a 8-bit number to the #3 message queue --- */
            status_04 = msgsnd(msqid_04, (struct msgbuf *)&buf_04, sizeof(buf_04.mtext[0]), 0);

            /* detect a message transmission error ------------- */
            if (status_04 <= -1)
            {
                printf("sending a number over the 4th queue failed ... \n\a");
            }
            else
            {
                printf("Odd number Sent:       %d\n", eight_bit_num);
            }
            //break;
        }
        else
        {
            // discard the even number (do nothing)
            printf("Even number discarded:       %d\n", eight_bit_num);
        }
    }
    //delete the queue
    msgctl(msqid_04, IPC_RMID, NULL);
    printf("OMM queue deleted...\n");
    exit(0);
}