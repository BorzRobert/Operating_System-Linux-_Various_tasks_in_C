#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"

int main()
{
    int pid;

    init();
    info(BEGIN, 1, 0);
    //printf("asta face P1 initial\n");
    pid = fork();

    if (pid == -1)
        printf("Error creating child process!\n");
    else
    {
        if (pid == 0) // child process(P2)
        {
            int pid1, pid2;
            info(BEGIN, 2, 0);
            //printf("Asta face P2 initial\n");
            pid1 = fork();
            if (pid1 == -1)
                printf("Error creating child process!\n");
            else
            {
                if (pid1 == 0) // child process(P3)
                {
                    int pid3, pid4, pid5, pid6;
                    info(BEGIN, 3, 0);
                    //printf("Asta face P3 initial\n");
                    pid3 = fork();
                    if (pid3 == -1)
                        printf("Error creating child process!\n");
                    else
                    {
                        if (pid3 == 0) // child process(P5)
                        {
                            info(BEGIN, 5, 0);
                            //printf("Asta face P5\n");
                            info(END, 5, 0);
                        }
                        else // parent process
                        {
                            int status;
                            wait(&status);
                            //printf("asta P3 dupa ce P5 se termina\n");
                            pid4 = fork();
                            if (pid4 == -1)
                                printf("Error creating child process!\n");
                            else
                            {
                                if (pid4 == 0) // child process(P6)
                                {
                                    info(BEGIN, 6, 0);
                                    //printf("Asta face P6\n");
                                    info(END, 6, 0);
                                }
                                else // parent process
                                {
                                    int status;
                                    wait(&status);
                                    //printf("asta P3 dupa ce P6 se termina\n");
                                    pid5 = fork();
                                    if (pid5 == -1)
                                        printf("Error creating child process!\n");
                                    else
                                    {
                                        if (pid5 == 0) // child process(P7)
                                        {
                                            info(BEGIN, 7, 0);
                                            //printf("Asta face P7\n");
                                            info(END, 7, 0);
                                        }
                                        else // parent process
                                        {
                                            int status;
                                            wait(&status);
                                            //printf("asta P3 dupa ce P7 se termina\n");
                                            pid6 = fork();
                                            if (pid6 == -1)
                                                printf("Error creating child process!\n");
                                            else
                                            {
                                                if (pid6 == 0) // child process(P8)
                                                {
                                                    info(BEGIN, 8, 0);
                                                    //printf("Asta face P8\n");
                                                    info(END, 8, 0);
                                                }
                                                else // parent process
                                                {
                                                    int status;
                                                    wait(&status);
                                                    //printf("asta P3 dupa ce P8 se termina\n");
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
                    //printf("asta P2 dupa ce P3 se termina\n");

                    pid2 = fork();
                    if (pid2 == -1)
                        printf("Error creating child process!\n");
                    else
                    {
                        if (pid2 == 0) // child process(P4)
                        {
                            info(BEGIN, 4, 0);
                            //printf("Asta face P4\n");
                            info(END, 4, 0);
                        }
                        else // parent process
                        {
                            int status;
                            wait(&status);
                            //printf("asta P2 dupa ce P4 se termina\n");
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
            //printf("asta P1 dupa ce P2 se termina\n");
            info(END, 1, 0);
        }
    }

    return 0;
}
