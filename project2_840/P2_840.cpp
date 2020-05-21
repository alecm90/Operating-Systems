/* ********************************************************** *
 * CS314 Project #2 solution                                  *
 *                                                            *
 * Your last-three: 840                                       *
 * Your course section #: 314-002                             *
 *                                                            *
 * Spring 2020                                                *
 *                                                            *
 * You can "copy & paste" this to your source code file       *
 *                                                            *
 * ********************************************************** */
#define NUM_REPEAT 50 // each boiler-man repeats

#define BATHER_TIME_01_A 300 // 300ms = 0.3 seconds
#define BATHER_TIME_01_B 800 // 800ms = 0.8 seconds

#define BATHER_TIME_02_A 300 // 300ms = 0.3 seconds
#define BATHER_TIME_02_B 800 // 800ms = 0.8 seconds

#define BATHER_TIME_03_A 300 // 300ms = 0.3 seconds
#define BATHER_TIME_03_B 800 // 800ms = 0.8 seconds

#define BOLIERMAN_TIME_01_A 1200 // 1200ms = 1.2 seconds
#define BOLIERMAN_TIME_01_B 1600 // 1600ms = 1.6 seconds

#define BOLIERMAN_TIME_02_A 1200 // 1200ms = 1.2 seconds
#define BOLIERMAN_TIME_02_B 1600 // 1600ms = 1.6 seconds

#define SEM_KEY_A 5882 // the semaphore key A
#define SEM_KEY_B 5883 // the semaphore key

#define SHM_KEY_A 7584 // the shared memory key
#define SHM_KEY_B 7583 // the shared memory key


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// the followings are for semaphores
#include <sys/sem.h>
#include <sys/ipc.h>

// the followings are for shared memory
#include <sys/shm.h>

#define NUM_BATHERS 3 
#define NUM_BOILERMEN 2
#define NUM_CHILDREN 4


int main(void)
{

    pid_t process_id;
    int i;                       // external loop counter
    int j;                       // internal loop counter
    int k = 0;                   // dumy integer

    int sleep_time;              //time to sleep/stay in critical section

    int sem_id_1;                  // the semaphore ID
    int sem_id_2;                  // the semaphore ID

    struct sembuf operations[1]; // Define semaphore operations
    int ret_val;                 // system-call return value

    int shm_id;                  // the shared memory ID
    int shm_size;                // the size of the shared memoy
    struct my_mem *p_shm;        // pointer to the attached shared memory

    int my_BID = 0;                  // id for the boilermen
    int my_TID = 0;                  // id for the bathers

    int batherTimesArray_A [] = {
        BATHER_TIME_01_A, BATHER_TIME_02_A, BATHER_TIME_03_A, 
    };
    int batherTimesArray_B [] = {
        BATHER_TIME_01_B, BATHER_TIME_02_B, BATHER_TIME_03_B, 
    };

    int boilerTimesArray_A [] = {
        BOLIERMAN_TIME_01_A, BOLIERMAN_TIME_02_A, 
    };
    int boilerTimesArray_B [] = {
        BOLIERMAN_TIME_01_B, BOLIERMAN_TIME_02_B
    };


    // Semaphore control data structure ----
    union semun {
        int val;
        struct semid_ds *buf;
        ushort *arry;
    } argument;
    argument.val = 1; // the initial value of the semaphore

    // shared memory definition ----
    struct my_mem
    {
        long int counter;
        int parent;
        int child;
        int boilManCounter;
        int batherCounter;
    };

    // find the shared memory size in bytes ----
    shm_size = sizeof *p_shm;
    if (shm_size <= 0)
    {
        fprintf(stderr, "sizeof error in acquiring the shared memory size. Terminating ..\n");
        exit(0);
    }



    // create/initialize shared memory ---- (1) -------------------------------------------
    shm_id = shmget(SHM_KEY_B, shm_size, 0666 | IPC_CREAT);
    if (shm_id < 0)
    {
        fprintf(stderr, "Failed to create the shared memory. Terminating ..\n");
        exit(0);
    }

    // attach the new shared memory
    p_shm = (struct my_mem *)shmat(shm_id, NULL, 0);
    if (p_shm == (struct my_mem *)-1)
    {
        fprintf(stderr, "Failed to attach the shared memory.  Terminating ..\n");
        exit(0);
    }

    // initialize the shared memory
    p_shm->counter = 0;
    p_shm->parent = 0;
    p_shm->child = 0;
    p_shm->boilManCounter = 0;
    p_shm->batherCounter = 0;


    // create/initialize the 2 new semaphores ----- (2) ----------------------------------------
    // Semaphore A ------------------
    sem_id_1 = semget(SEM_KEY_A, 1, 0666 | IPC_CREAT);
    if (sem_id_1 < 0)
    {
        fprintf(stderr, "Failed to create a new semaphore. Terminating ..\n");
        exit(0);
    }

    // initialzie Semaphore A by 1 (zero)
    if (semctl(sem_id_1, 0, SETVAL, argument) < 0)
    {
        fprintf(stderr, "Failed to initialize the semapgore by 1. Terminating ..\n");
        exit(0);
    }
    // Semaphore B ------------------
    sem_id_2 = semget(SEM_KEY_B, 2, 0666 | IPC_CREAT);
    if (sem_id_2 < 0)
    {
        fprintf(stderr, "Failed to create a new semaphore. Terminating ..\n");
        exit(0);
    }

    // initialzie Semaphore B by 1 (zero)
    if (semctl(sem_id_2, 0, SETVAL, argument) < 0)
    {
        fprintf(stderr, "Failed to initialize the semapgore by 1. Terminating ..\n");
        exit(0);
    }
    // ----------------------------------------------------------------------------------------



    // create the first child process (B2) ---- (3) -----------------------------------------------------
    process_id = fork();



    // 1st child process, Boilerman 2 (B2) -------------------------------------------------------------
    if (process_id == 0)
    {
        my_BID = 2;

        // give process_ID value 1, so it doesnt go through bather code
        process_id = 1;

        //printf("BoilerMan %d :  WAITING til all children created...\n", my_BID);

        // initialize child process ----
        p_shm->child = p_shm->child++; // child started ..

        // Block until all children are created ----
        while (p_shm->child != 0)
        {
            k = k + 1;
            //printf("B2 p_shm->child = %d  ", p_shm->child);
        }
        k = 0;


        //printf("BoilerMan %d :  READY to enter critical section...\n", my_BID);

        // declare completion ----
        p_shm->child = p_shm->child--;


        //exit(0);
    
    }
    // Create the number of bather processes
    for (int i = 0; i < NUM_BATHERS; i++){
    // parent process (B1), create next child processes (T1-)
        if (process_id > 0) {

            process_id = fork();
            my_TID = i + 1;

        }
        // else process_id < 0, error
        else if (process_id < 0)
        {
            printf("Error: fork() not successfull...\n");
            exit(0);
        }


    }



// if pID > 0, Boilerman process (B1, B2) ------------------------------------------------------
    if (process_id > 0 )
    {
        // Original parent process pID > 0, So BID = 1
        if (my_BID == 0)
        {
            my_BID = 1;
            // signal to children that all processes are created
            //printf("BoilerMan %d parent process: WAITING for children to get ready ...\n", my_BID);

            // initialize the parent process ----
            p_shm->parent = 1; // the parent process started ..
        }



        // wait for the child process to get ready ----
        while (p_shm->child != 0)
        {
            k = k + 1;
            //printf("B1");
            //printf("p_shm->child = %d", p_shm->child);
            printf("");
        }
        k = 0;

        // Boilers repeat NUM_Repeat times
        for (i = 0; i < NUM_REPEAT; i++)
        {
            
            // Make the Bather process sleep for a RAND time
            sleep_time = rand() % boilerTimesArray_A[my_BID - 1];
            usleep(sleep_time);

            // // Siginal that a Boiler is in the critical section
            // p_shm->boilManCounter =  p_shm->boilManCounter++;

            // WAIT on the semaphore -----
            operations[0].sem_num = 0; // the first semapahore
            operations[0].sem_op = -1; // "wait" on the semaphore
            operations[0].sem_flg = 0; // make sure to block
            ret_val = semop(sem_id_1, operations, 1);
            if (ret_val != 0)
            {
                fprintf(stderr, "P-OP (wait) failed ....\a\n");
            }


            // Siginal that a Boiler is in the critical section
            p_shm->boilManCounter =  p_shm->boilManCounter++;

            // wait for the bathers to leave critical section
            while (p_shm->batherCounter > 0)
            {
                k = k + 1;
                //printf("B1");
                //printf("p_shm->child = %d", p_shm->child);
                printf("");
            }
            k = 0;


            // // Siginal that a Boiler is in the critical section
            // p_shm->boilManCounter =  p_shm->boilManCounter++;

            // Wait a couple seconds for everything to print
            for (j = 0; j < 100000; j++)
            {
                k = k + 1;
            }

            // CRITICAL SECTION STARTS HERE --------------------------------------
            printf("B%d Starts his boiler..\n", my_BID);

            //printf("p_shm->boilManCounter = %d\n", p_shm->boilManCounter);

            // Make the Bather process sleep for a RAND time
            sleep_time = boilerTimesArray_B[my_BID - 1];
            usleep(sleep_time);
            printf("B%d is leaving the bathing area..\n", my_BID);


            // SIGNAL on the semapahore ----
            operations[0].sem_num = 0;
            operations[0].sem_op = 1; // SIGNAL
            operations[0].sem_flg = 0;
            ret_val = semop(sem_id_1, operations, 1);
            if (ret_val != 0)
            {
                fprintf(stderr, "V-OP (signal) failed ....\a\n");
            }
            // Siginal that a Boiler has left critical section
            p_shm->boilManCounter =  p_shm->boilManCounter--;



        }

        // declare boilers completion ----
        p_shm->parent = 0;

        // Wait a couple seconds for everything to print
        for (j = 0; j < 100000000; j++)
        {
            k = k + 1;
        }

        if (my_BID > 1) {
            printf("----- Boilermen finished repeating %d times -----\n", NUM_REPEAT);
        }

        // wait for the child process to complete ----
        k = 0;
        while (p_shm->child != 0)
        {
            k = k + 1;
        }





        // Delete the shared memory -------------------------------------------------------------
        if (process_id > 0 && my_BID == 1)
        {
            printf("DELETING SHARED MEM\n");

            // detach the shared memory ---
            ret_val = shmdt(p_shm);
            if (ret_val != 0)
            {
                printf("shared memory detach failed ....\n");
            }

            ret_val = shmctl(shm_id, IPC_RMID, 0);
            if (ret_val != 0)
            {
                printf("shared memory ID remove ID failed ... \n");
            }

            ret_val = semctl(sem_id_1, IPC_RMID, 0);
            if (ret_val != 0)
            {
                printf("semaphore remove ID failed ... \n");
            }
            ret_val = semctl(sem_id_2, IPC_RMID, 0);
            if (ret_val != 0)
            {
                printf("semaphore remove ID failed ... \n");
            }
        }
        exit(0);
    }



    // BATHER child processes, (not B2 though) (T1-T3) -----------------------------------------------------------
    if (process_id == 0 && my_BID == 0) {


        //printf("Bather %d    :  WAITING til all children created...\n", my_TID);

        // initialize child process ----
        p_shm->child = p_shm->child++; // child started ..
        //printf("This is p_shm->child = %d \n", p_shm->child);


        // Block untill all children are created -------------------------------------
        while (p_shm->child != 0)
        {
            k = k + 1;
            //printf("Bather p_shm->child = %d  ", p_shm->child);
        }
        k = 0;


        //printf("Bather %d    :  READY to enter critical section...\n", my_TID);

        // Wait a couple seconds for everything to print
        for (j = 0; j < 10000000; j++)
        {
            k = k + 1;
        }

 
        // while no boilers are active (B1,B2 is not active)
        while (p_shm->boilManCounter == 0 )
        {


            // Make the Bather process sleep for a RAND time
            sleep_time = rand() % batherTimesArray_A[my_TID - 1];
            usleep(sleep_time);

            // add 1 to the shared memory ----
            p_shm->batherCounter = p_shm->batherCounter++;

            // if there are boilers, bathers cannot enter
            if (p_shm->boilManCounter > 0) {
                printf("p_shm->boilManCounter = %d", p_shm->boilManCounter);
                // WAIT on the semaphore ----
                operations[0].sem_num = 0;
                operations[0].sem_op = -1; // "wait" on the semaphore
                operations[0].sem_flg = 0;
                ret_val = semop(sem_id_2, operations, 1);
                if (ret_val != 0)
                {
                    fprintf(stderr, "P-OP(wait) failed (child) ....\a\n");
                }
            }
            else {
                // SIGNAL on the semaphore ----
                operations[0].sem_num = 0;
                operations[0].sem_op = 1;
                operations[0].sem_flg = 0;
                ret_val = semop(sem_id_2, operations, 1);
                if (ret_val != 0)
                {
                    fprintf(stderr, "V-OP (wait) failed (child) ....\a\n");
                }
            }

            // Used to break the while loop if the boilers are done repeating
            if (p_shm->parent == 0 ||p_shm->boilManCounter > 0) {
                break;
            }
            // CRITICAL SECTION STARTS HERE --------------------------------------
            printf("T%d is entering the bathing area..\n", my_TID);



            // Make the Bather process sleep for a RAND time
            sleep_time = batherTimesArray_B[my_TID - 1];
            usleep(sleep_time);
            printf("T%d is leaving the bathing area..\n", my_TID);


            for (j = 0; j < 10000; j++)
            {
                k = k + 1;
            }

            // SIGNAL on the semaphore ----
            operations[0].sem_num = 0;
            operations[0].sem_op = 1;
            operations[0].sem_flg = 0;
            ret_val = semop(sem_id_2, operations, 1);
            if (ret_val != 0)
            {
                fprintf(stderr, "V-OP (wait) failed (child) ....\a\n");
            }

            // subtract 1 from the shared memory ----
            p_shm->batherCounter = p_shm->batherCounter--;

            // CRITICAL SECTION ENDS HERE ----------------------------------------

        }

        // declare completion ----
        //printf("p_shm->child now equals = %d \n", p_shm->child);
        p_shm->child = p_shm->child--;
        //printf("p_shm->child now equals = %d \n", p_shm->child);


        // wait for B1 and B2 to complete ---
        k = 0;
        while (p_shm->parent != 0)
        {
            k = k + 1;
            //printf("T%d", my_TID);
        }

        //exit(0);
    }


    exit(0);
}

