#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "a2_helper.h"

#define NUM_THREADS 46
#define MAX_THREADS_RUNNING 6

sem_t semaphore;
int ok15 = 0;

void *thread_function_P5(void *arg)
{
    info(BEGIN, 5, *((int *)arg));
    // printf("Asta face T5.%i\n", *((int *)arg));
    info(END, 5, *((int *)arg));
    return 0;
}
void *thread_function_P5_four(void *arg)
{
    info(BEGIN, 5, *((int *)arg));
    return 0;
}
void *thread_function_P8(void *arg)
{
    info(BEGIN, 8, *((int *)arg));
    // printf("Asta face T5.%i\n", *((int *)arg));
    info(END, 8, *((int *)arg));
    sem_post(&semaphore);
    return 0;
}
void *thread_function_P8_1_5(void *arg)
{
    info(BEGIN, 8, *((int *)arg));
    // printf("Asta face T5.%i\n", *((int *)arg));
    while (ok15 == 0)
    {
        // wait
    }
    info(END, 8, *((int *)arg));
    sem_post(&semaphore);
    return 0;
}
void *T15_function(void *arg)
{
    info(BEGIN, 8, *((int *)arg));
    info(END, 8, *((int *)arg));
    ok15 = 1;
    sem_post(&semaphore);
    return 0;
}
void *thread_function_P6(void *arg)
{
    info(BEGIN, 6, *((int *)arg));
    // printf("Asta face P6.%i\n", *((int *)arg));
    info(END, 6, *((int *)arg));
    return 0;
}
int main()
{
    int pid, pid1, pid2, pid3, pid4, pid5, pid6, status;
    sem_init(&semaphore, 0, MAX_THREADS_RUNNING);
    init();
    info(BEGIN, 1, 0);
    // printf("asta face P1 initial\n");
    pid = fork();
    switch (pid)
    {
    case -1:
        printf("Error creating child process!\n");
        exit(1);
        break;
    case 0: // child process(P2)
        info(BEGIN, 2, 0);
        // printf("Asta face P2 initial\n");
        pid1 = fork();
        switch (pid1)
        {
        case -1:
            printf("Error creating child process!\n");
            exit(1);
            break;
        case 0: // child process(P3)

            info(BEGIN, 3, 0);
            // printf("Asta face P3 initial\n");
            pid3 = fork();
            switch (pid3)
            {
            case -1:
                printf("Error creating child process!\n");
                exit(1);
                break;
            case 0: // child process(P5)
                info(BEGIN, 5, 0);
                // printf("Asta face P5\n");
                pthread_t thread1, thread2, thread3, thread4, thread5, thread3_6;
                int one = 1, two = 2, three = 3, four = 4, five = 5;
                if (pthread_create(&thread1, NULL, thread_function_P5, (void *)&one) != 0)
                    printf("Error creating P5.1\n");
                else
                {
                    // printf("Am creat threadul P5.1 si l-am si inchis\n");
                    pthread_join(thread1, NULL);
                }
                if (pthread_create(&thread3, NULL, thread_function_P5, (void *)&three) != 0)
                    printf("Error creating P5.3\n");
                else
                {
                    // printf("Am creat threadul P5.3 si l-am si inchis\n");
                    pthread_join(thread3, NULL);
                }
                pthread_create(&thread3_6, NULL, thread_function_P6, (void *)&three);
                pthread_join(thread3_6, NULL);
                if (pthread_create(&thread5, NULL, thread_function_P5, (void *)&five) != 0)
                    printf("Error creating P5.5\n");
                else
                {
                    // printf("Am creat threadul P5.5 si l-am si inchis\n");
                    pthread_join(thread5, NULL);
                }
                pthread_create(&thread4, NULL, thread_function_P5_four, (void *)&four);
                pthread_create(&thread2, NULL, thread_function_P5_four, (void *)&two);
                pthread_join(thread2, NULL);
                info(END, 5, 2);
                pthread_join(thread4, NULL);
                info(END, 5, 4);
                info(END, 5, 0);
                break;

            default:
                wait(&status);
                // printf("asta face P3 dupa ce P5 se termina\n");
                pid4 = fork();
                switch (pid4)
                {
                case -1:
                    printf("Error creating child process!\n");
                    exit(1);
                    break;
                case 0: // child process(P6)
                    info(BEGIN, 6, 0);
                    // printf("Asta face P6\n");
                    int threadId[6];
                    pthread_t threads[6];
                    for (int i = 0; i < 6; i++)
                        threadId[i] = i + 1;
                    if (pthread_create(&threads[0], NULL, thread_function_P6, (void *)&threadId[0]) != 0)
                        printf("Error creating P6.1\n");
                    else
                    {
                        // printf("Am creat threadul P6.1 si l-am si inchis\n");
                        pthread_join(threads[0], NULL);
                    }
                    if (pthread_create(&threads[1], NULL, thread_function_P6, (void *)&threadId[1]) != 0)
                        printf("Error creating P6.2\n");
                    else
                    {
                        // printf("Am creat threadul P6.2 si l-am si inchis\n");
                        pthread_join(threads[1], NULL);
                    }
                    if (pthread_create(&threads[3], NULL, thread_function_P6, (void *)&threadId[3]) != 0)
                        printf("Error creating P6.4\n");
                    else
                    {
                        // printf("Am creat threadul P6.4 si l-am si inchis\n");
                        pthread_join(threads[3], NULL);
                    }
                    if (pthread_create(&threads[4], NULL, thread_function_P6, (void *)&threadId[4]) != 0)
                        printf("Error creating P6.5\n");
                    else
                    {
                        // printf("Am creat threadul P6.5 si l-am si inchis\n");
                        pthread_join(threads[4], NULL);
                    }
                    if (pthread_create(&threads[5], NULL, thread_function_P6, (void *)&threadId[5]) != 0)
                        printf("Error creating P6.6\n");
                    else
                    {
                        // printf("Am creat threadul P6.6 si l-am si inchis\n");
                        pthread_join(threads[5], NULL);
                    }
                    info(END, 6, 0);
                    break;
                default:
                    wait(&status);
                    // printf("asta P3 dupa ce P6 se termina\n");
                    pid5 = fork();
                    switch (pid5)
                    {
                    case -1:
                        printf("Error creating child process!\n");
                        exit(1);
                        break;
                    case 0: // child process(P7)
                        info(BEGIN, 7, 0);
                        // printf("Asta face P7\n");
                        info(END, 7, 0);
                        break;

                    default:
                        wait(&status);
                        // printf("asta P3 dupa ce P7 se termina\n");
                        pid6 = fork();
                        switch (pid6)
                        {
                        case -1:
                            printf("Error creating child process!\n");
                            exit(1);
                            break;
                        case 0: // child process(P8)
                            info(BEGIN, 8, 0);
                            int threadId[NUM_THREADS];
                            for (int i = 0; i < 46; i++)
                                threadId[i] = i + 1;
                            pthread_t threads[NUM_THREADS];
                            for (int i = 0; i < 5; i++)
                            {
                                sem_wait(&semaphore);
                                pthread_create(&threads[i], NULL, thread_function_P8_1_5, (void *)&threadId[i]);
                            }
                            for (int i = 5; i < 46; i++)
                            {
                                sem_wait(&semaphore);
                                if (i == 14)
                                {
                                    pthread_create(&threads[i], NULL, T15_function, (void *)&threadId[i]);
                                }
                                else
                                    pthread_create(&threads[i], NULL, thread_function_P8, (void *)&threadId[i]);
                            }
                            for (int i = 0; i < NUM_THREADS; i++)
                            {
                                pthread_join(threads[i], NULL);
                            }
                            sem_destroy(&semaphore);
                            // printf("Asta face P8\n");
                            info(END, 8, 0);
                            break;

                        default:
                            wait(&status);
                            // printf("asta P3 dupa ce P8 se termina\n");
                            info(END, 3, 0);
                            break;
                        }
                        break;
                    }

                    break;
                }
                break;
            }
            break;

        default:
            wait(&status);
            // printf("asta face P2 dupa ce P3 se termina\n");

            pid2 = fork();
            switch (pid2)
            {
            case -1:
                printf("Error creating child process!\n");
                exit(1);
                break;
            case 0: // child process(P4)
                info(BEGIN, 4, 0);
                // printf("Asta face P4\n");
                info(END, 4, 0);
                break;

            default:
                wait(&status);
                // printf("asta P2 dupa ce P4 se termina\n");
                info(END, 2, 0);
                break;
            }
            break;
        }
        break;

    default:
        wait(&status);
        // printf("asta P1 dupa ce P2 se termina\n");
        info(END, 1, 0);
        break;
    }
}
