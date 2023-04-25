#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"

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
int main()
{
    int pid;

    init();
    info(BEGIN, 1, 0);
    // printf("asta face P1 initial\n");
    pid = fork();

    if (pid == -1)
        printf("Error creating child process!\n");
    else
    {
        if (pid == 0) // child process(P2)
        {
            int pid1, pid2;
            info(BEGIN, 2, 0);
            // printf("Asta face P2 initial\n");
            pid1 = fork();
            if (pid1 == -1)
                printf("Error creating child process!\n");
            else
            {
                if (pid1 == 0) // child process(P3)
                {
                    int pid3, pid4, pid5, pid6;
                    info(BEGIN, 3, 0);
                    // printf("Asta face P3 initial\n");
                    pid3 = fork();
                    if (pid3 == -1)
                        printf("Error creating child process!\n");
                    else
                    {
                        if (pid3 == 0) // child process(P5)
                        {
                            info(BEGIN, 5, 0);
                            // printf("Asta face P5\n");
                            pthread_t thread1, thread2, thread3, thread4, thread5;
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
                                // printf("Am creat threadul P5.2 si l-am si inchis\n");
                                pthread_join(thread3, NULL);
                            }
                            if (pthread_create(&thread5, NULL, thread_function_P5, (void *)&five) != 0)
                                printf("Error creating P5.5\n");
                            else
                            {
                                // printf("Am creat threadul P5.2 si l-am si inchis\n");
                                pthread_join(thread5, NULL);
                            }
                            pthread_create(&thread4, NULL, thread_function_P5_four, (void *)&four);
                            pthread_create(&thread2, NULL, thread_function_P5, (void *)&two);
                            pthread_join(thread2, NULL);
                            pthread_join(thread4, NULL);
                            info(END,5,4);

                            info(END, 5, 0);
                        }
                        else // parent process
                        {
                            int status;
                            wait(&status);
                            // printf("asta P3 dupa ce P5 se termina\n");
                            pid4 = fork();
                            if (pid4 == -1)
                                printf("Error creating child process!\n");
                            else
                            {
                                if (pid4 == 0) // child process(P6)
                                {
                                    info(BEGIN, 6, 0);
                                    // printf("Asta face P6\n");
                                    info(END, 6, 0);
                                }
                                else // parent process
                                {
                                    int status;
                                    wait(&status);
                                    // printf("asta P3 dupa ce P6 se termina\n");
                                    pid5 = fork();
                                    if (pid5 == -1)
                                        printf("Error creating child process!\n");
                                    else
                                    {
                                        if (pid5 == 0) // child process(P7)
                                        {
                                            info(BEGIN, 7, 0);
                                            // printf("Asta face P7\n");
                                            info(END, 7, 0);
                                        }
                                        else // parent process
                                        {
                                            int status;
                                            wait(&status);
                                            // printf("asta P3 dupa ce P7 se termina\n");
                                            pid6 = fork();
                                            if (pid6 == -1)
                                                printf("Error creating child process!\n");
                                            else
                                            {
                                                if (pid6 == 0) // child process(P8)
                                                {
                                                    info(BEGIN, 8, 0);
                                                    // printf("Asta face P8\n");
                                                    info(END, 8, 0);
                                                }
                                                else // parent process
                                                {
                                                    int status;
                                                    wait(&status);
                                                    // printf("asta P3 dupa ce P8 se termina\n");
                                                    info(END, 3, 0);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else // parent process(P2)
                {
                    int status;
                    wait(&status);
                    // printf("asta P2 dupa ce P3 se termina\n");

                    pid2 = fork();
                    if (pid2 == -1)
                        printf("Error creating child process!\n");
                    else
                    {
                        if (pid2 == 0) // child process(P4)
                        {
                            info(BEGIN, 4, 0);
                            // printf("Asta face P4\n");
                            info(END, 4, 0);
                        }
                        else // parent process
                        {
                            int status;
                            wait(&status);
                            // printf("asta P2 dupa ce P4 se termina\n");
                            info(END, 2, 0);
                        }
                    }
                }
            }
        }
        else // parent process
        {
            int status;
            wait(&status);
            // printf("asta P1 dupa ce P2 se termina\n");
            info(END, 1, 0);
        }
    }

    return 0;
}
