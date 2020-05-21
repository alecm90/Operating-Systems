/* *************************************************** *
 * emm.cpp:                                            *
 * even middle man                                     *
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

#define MSG_key_01 5881 // (unique) message queue key #1
#define MSG_key_03 5883 // (unique) message queue key #3

#define BUFFER_SIZE 256 // max. message size

int main(void)
{
    int i; // loop counter

    int status_01; // result status for (#1)
    int status_03; // result status for (#3)

    int msqid_01; // message queue ID (#1)
    int msqid_03; // message queue ID (#3)

    key_t msgkey_01; // message-queue key (#1)
    key_t msgkey_03; // message-queue key (#3)

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
    struct message buf_01; // for #1
    struct message buf_03; // for #3

    msgkey_01 = MSG_key_01; // the messge-que ID key (#1)
    msgkey_03 = MSG_key_03; // the messge-que ID key (#3)

    /* create a new message queue (#1 from sender)-------------- */
    msqid_01 = msgget(msgkey_01, 0666 | IPC_CREAT);

    /* create a new message queue (#3 to reciever)-------------- */
    msqid_03 = msgget(msgkey_03, 0666 | IPC_CREAT);

    /* error check ----------------------------- */
    if ((msqid_01 <= -1) || (msqid_03 <= -1))
    {
        if (msqid_01 <= -1)
        {
            printf("Your new message queue (#1) was not created ....\n");
        }
        else
        {
            printf("Your new message queue (#3) was not created ....\n");
        }

        printf("Process is terminating ... \n\a");
    }
    else
    {
        printf("Your new message queues are successfully created ....\n");
    }

    buf_01.mtype = 1; // UNIX standard says, any number

    for (int i = 0; i < NUM_REPEATS; i++)
    {

        // recieve message from sender
        status_01 = msgrcv(msqid_01, (struct msgbuf *)&buf_01, sizeof(buf_01), 1, 0);

        // number being recived
        eight_bit_num = buf_01.mtext[0];

        /* display number recieved --------------------------*/
        //printf("Num Recieved is: %d \n", eight_bit_num);

        /* if even, send to the reciever; if odd, discard ----*/
        if ((eight_bit_num % 2) == 0)
        {
            // send to receiver
            /* set the new eight-bit number to the message buffers ------ */
            buf_03.mtext[0] = eight_bit_num; // copy the 8-bit number
            buf_03.mtext[1] = '\0';          // null-terminate it

            /* send a 8-bit number to the #3 message queue --- */
            status_03 = msgsnd(msqid_03, (struct msgbuf *)&buf_03, sizeof(buf_03.mtext[0]), 0);

            /* detect a message transmission error ------------- */
            if (status_03 <= -1)
            {
                printf("Sending a number over the 3st queue failed ... \n\a");
            }
            else
            {
                printf("Even number sent:       %d\n", eight_bit_num);
            }
            //break;
        }
        else
        {
            // discard the odd number (do nothing)
            printf("Odd number discarded:       %d\n", eight_bit_num);
        }
    }
    //delete the queue
    msgctl(msqid_03, IPC_RMID, NULL);
    printf("EMM queue deleted...\n");
    exit(0);
}