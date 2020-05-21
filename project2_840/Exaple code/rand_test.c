/* ******************************************** *
 * rand_test.c:                                 *
 *                                              *
 *   A random number generator.                 *
 *                                              *
 *   It sleeps for a number of second(s)        *
 *   generated by the random-number generator.  *
 *                                              *
 *   The max. random number value is controlled *
 *   by a label: MAX_RAND (defined below).      *
 *                                              *
 *   It does not change the random seed.        *
 *                                              *
 *   Compile: gcc reand_test.cpp                *
 *                                              *
 *   Host: os.cs.siue.edu                       *
 *                                              *
 *   2/2/2020                                   *
 * ******************************************** */

#include <stdio.h>          // for printf, rand
#include <stdlib.h>         // for printf, rand
#include <sys/types.h>      // for printf, rand
#include <unistd.h>         // for sleep

#define  NUM_REPEATS   10      // the number of repeats
#define  MAX_RAND      30000  // the max. random number  

int main (void)
{
   int i;               // loop counter

   long int my_rand;    // a random number
   long int temp1;

   for (i = 0; i < NUM_REPEATS; i++)
   {
      temp1 = (MAX_RAND/1000);
      my_rand = (rand() % temp1);
      my_rand =  my_rand * 100;

      printf("I am sleeping for %d micro-second(s) ...\n", my_rand);

      usleep(my_rand);
   }

   exit (1);
} 