/* ********************************************** *
 * fork_test.c                                    *
 *                                                *
 *  For an example of "fork" system call.         *
 *                                                *
 *  The first (parent) process creates a child    *
 *  (duplicated) process.  Each of them repeats   *
 *  as many as NUM_REPEATS times.                 *
 *                                                *
 *  gcc fork_test.c                               *
 *                                                *
 *  os.cs.siue.edu  2/1/2020                      *
 * ********************************************** */
#include <stdio.h>          // for printf, rand
#include <unistd.h>         // for sleep
#include <sys/types.h>      // for type "pid_t"
#include <stdlib.h> 

#define NUM_REPEATS   100   // number of repeats
#define  MAX_RAND       5   // the max. random number  


int main (void)
{
   int i;                   // loop counter
   int my_rand;             // a random number

   pid_t  process_id;       // process ID holer


   process_id = fork();

   // The child process ------------------------- //
   if (process_id == 0)
   {
        printf("I am the child process ...\n");

        for (i = 0; i < NUM_REPEATS; i++)
        {
            my_rand = (rand() % MAX_RAND);

            printf("the child process is about to sleep for %d seconds ...\n", my_rand);

            sleep(my_rand);

            printf("the child process wakes up ...\n");

            my_rand = (rand() % MAX_RAND);

            sleep(my_rand);
        }
 
     return(0);  
  }

   // The parent process ------------------------- //
   else
   {
        printf("I am the parent process ...\n");

        for (i = 0; i < NUM_REPEATS; i++)
        {
            my_rand = (rand() % MAX_RAND);

            printf("the parent process is about to sleep for %d seconds ...\n", my_rand);

            sleep(my_rand);

            printf("the parent process wakes up ...\n");

            my_rand = (rand() % MAX_RAND);

            sleep(my_rand);
        }
    }

    return(1);  
}

/* END OF THE LINES ----------------------------- */