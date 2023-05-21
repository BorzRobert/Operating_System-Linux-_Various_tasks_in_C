#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

#define MAX_BUFFER_SIZE 1024

int main()
{
      const char *reqPipeName = "REQ_PIPE_81069";
      const char *respPipeName = "RESP_PIPE_81069";
      int respPipefd, reqPipefd, i;

      char *message = malloc(MAX_BUFFER_SIZE * sizeof(char));

      if (mkfifo(respPipeName, 0666) == -1)
      {
            printf("ERROR\ncannot create the response pipe\n");
            exit(1);
      }
      reqPipefd = open(reqPipeName, O_RDWR);
      if (reqPipefd == -1)
      {
            printf("ERROR\ncannot open the request pipe\n");
            exit(1);
      }
      respPipefd = open(respPipeName, O_RDWR);
      if (respPipefd == -1)
      {
            printf("ERROR\ncannot open the response pipe\n");
            close(reqPipefd);
            exit(1);
      }

      strcpy(message, "START#");
      ssize_t bytesWritten = write(respPipefd, message, strlen(message));
      if (bytesWritten == -1)
      {
            printf("ERROR\nwritting in the response pipe\n");
            close(reqPipefd);
            close(respPipefd);
            unlink(respPipeName);
            unlink(reqPipeName);
            exit(1);
      }
      printf("SUCCESS\n");

      int shm_fd, fd, len;
      unsigned int mem_reg_size;
      char *shared_char = NULL;
      char *shared_char_file = NULL;
      char request[MAX_BUFFER_SIZE];
      unsigned char byte;
      ssize_t bytes_read;
      struct stat sb;

      while (1)
      {
            i = 0;
            // Take first string value of the command and then read command's parameters
            while ((bytes_read = read(reqPipefd, &byte, sizeof(byte))) > 0 && byte != '#')
            {
                  request[i] = byte;
                  i++;
            }
            request[i] = '\0';

            if (strcmp(request, "EXIT") == 0) // Exit request
            {
                  close(reqPipefd);
                  close(respPipefd);
                  unlink(respPipeName);
                  unlink(reqPipeName);
                  break;
            }
            else if (strcmp(request, "VARIANT") == 0) // Variant request
            {
                  unsigned int variant_number = 81069;
                  strcpy(message, "VARIANT#");
                  bytesWritten = write(respPipefd, message, strlen(message)); //"VARIANT"
                  if (bytesWritten < 0)
                  {
                        printf("ERROR\nwritting in the response pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  bytesWritten = write(respPipefd, &variant_number, sizeof(variant_number)); // 81069
                  if (bytesWritten < 0)
                  {
                        printf("ERROR\nwritting in the response pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  strcpy(message, "VALUE#");
                  bytesWritten = write(respPipefd, message, strlen(message)); //"VALUE"
                  if (bytesWritten < 0)
                  {
                        printf("ERROR\nwritting in the response pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
            }
            else if (strcmp(request, "CREATE_SHM") == 0) // Create shm request
            {
                  bytes_read = read(reqPipefd, &mem_reg_size, sizeof(mem_reg_size));
                  if (bytes_read < 0)
                  {
                        printf("ERROR\nreading from the request pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  shm_fd = shm_open("/nGf5ScLj", O_CREAT | O_RDWR, 0664);
                  if (shm_fd < 0)
                  {
                        printf("ERROR\ncould not aquire shm");
                        strcpy(message, "CREATE_SHM#ERROR#");
                        bytesWritten = write(respPipefd, message, strlen(message));
                        if (bytesWritten < 0)
                        {
                              printf("ERROR\nwritting in the response pipe\n");
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                        exit(1);
                  }
                  ftruncate(shm_fd, mem_reg_size);
                  shared_char = (char *)mmap(0, mem_reg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                  if (shared_char == MAP_FAILED)
                  {
                        printf("Could not map the shared memory");
                        strcpy(message, "CREATE_SHM#ERROR#");
                        bytesWritten = write(respPipefd, message, strlen(message));
                        if (bytesWritten < 0)
                        {
                              printf("ERROR\nwritting in the response pipe\n");
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                        exit(1);
                  }
                  strcpy(message, "CREATE_SHM#SUCCESS#");
                  bytesWritten = write(respPipefd, message, strlen(message));
                  if (bytesWritten < 0)
                  {
                        printf("ERROR\nwritting in the response pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
            }
            else if (strcmp(request, "WRITE_TO_SHM") == 0) // Write to shm request
            {
                  unsigned int offset, value;
                  strcpy(message, "WRITE_TO_SHM#");
                  bytesWritten = write(respPipefd, message, strlen(message));
                  if (bytesWritten < 0)
                  {
                        printf("ERROR\nwritting in the response pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  bytes_read = read(reqPipefd, &offset, sizeof(offset));
                  if (bytes_read < 0)
                  {
                        printf("ERROR\nreading from the request pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  bytes_read = read(reqPipefd, &value, sizeof(value));
                  if (bytes_read < 0)
                  {
                        printf("ERROR\nreading from the request pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }

                  if (offset > 2434223 || offset < 0)
                  {
                        strcpy(message, "ERROR#");
                        bytesWritten = write(respPipefd, message, strlen(message));
                        if (bytesWritten < 0)
                        {
                              printf("ERROR\nwritting in the response pipe\n");
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                  }
                  else
                  {

                        strcpy(message, "SUCCESS#");
                        bytesWritten = write(respPipefd, message, strlen(message));
                        if (bytesWritten < 0)
                        {
                              printf("ERROR\nwritting in the response pipe\n");
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                        memcpy(shared_char + offset, &value, 4);
                  }
            }
            else if (strcmp(request, "MAP_FILE") == 0) // Map file request
            {
                  strcpy(message, "MAP_FILE#");
                  bytesWritten = write(respPipefd, message, strlen(message));
                  if (bytesWritten < 0)
                  {
                        printf("ERROR\nwritting in the response pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  i = 0;
                  while ((bytes_read = read(reqPipefd, &byte, sizeof(byte))) > 0 && byte != '#')
                  {
                        request[i] = byte;
                        i++;
                  }
                  request[i] = '\0';
                  fd = open(request, O_RDWR);
                  if (fd < 0)
                  {
                        printf("ERROR\ncould not open file\n");
                        strcpy(message, "ERROR#");
                        bytesWritten = write(respPipefd, message, strlen(message));
                        if (bytesWritten < 0)
                        {
                              printf("ERROR\nwritting in the response pipe\n");
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  else
                  {
                        if (fstat(fd, &sb) == -1)
                        {
                              printf("ERROR\ncould not get file size\n");
                              strcpy(message, "ERROR#");
                              bytesWritten = write(respPipefd, message, strlen(message));
                              if (bytesWritten < 0)
                              {
                                    printf("ERROR\nwritting in the response pipe\n");
                                    close(reqPipefd);
                                    close(respPipefd);
                                    unlink(respPipeName);
                                    unlink(reqPipeName);
                                    exit(1);
                              }
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                        len = sb.st_size;
                        shared_char_file = (char *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                        if (shared_char_file == MAP_FAILED)
                        {
                              printf("Could not map the file");
                              strcpy(message, "ERROR#");
                              bytesWritten = write(respPipefd, message, strlen(message));
                              if (bytesWritten < 0)
                              {
                                    printf("ERROR\nwritting in the response pipe\n");
                                    close(reqPipefd);
                                    close(respPipefd);
                                    unlink(respPipeName);
                                    unlink(reqPipeName);
                                    exit(1);
                              }
                              exit(1);
                        }
                        else
                        {
                              strcpy(message, "SUCCESS#");
                              bytesWritten = write(respPipefd, message, strlen(message));
                              if (bytesWritten < 0)
                              {
                                    printf("ERROR\nwritting in the response pipe\n");
                                    close(reqPipefd);
                                    close(respPipefd);
                                    unlink(respPipeName);
                                    unlink(reqPipeName);
                                    exit(1);
                              }
                        }
                  }
            }
            else if (strcmp(request, "READ_FROM_FILE_OFFSET") == 0)
            {
                  unsigned int offset, no_of_bytes;
                  strcpy(message, "READ_FROM_FILE_OFFSET#");
                  bytesWritten = write(respPipefd, message, strlen(message));
                  if (bytesWritten < 0)
                  {
                        printf("ERROR\nwritting in the response pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  bytes_read = read(reqPipefd, &offset, sizeof(offset));
                  if (bytes_read < 0)
                  {
                        printf("ERROR\nreading from the request pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  bytes_read = read(reqPipefd, &no_of_bytes, sizeof(no_of_bytes));
                  if (bytes_read < 0)
                  {
                        printf("ERROR\nreading from the request pipe\n");
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  if (offset + no_of_bytes > len)
                  {
                        strcpy(message, "ERROR#");
                        bytesWritten = write(respPipefd, message, strlen(message));
                        if (bytesWritten < 0)
                        {
                              printf("ERROR\nwritting in the response pipe\n");
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                        close(reqPipefd);
                        close(respPipefd);
                        unlink(respPipeName);
                        unlink(reqPipeName);
                        exit(1);
                  }
                  else
                  {
                        //char *read_addr = shared_char_file + offset;
                        //char *buffer = malloc(MAX_BUFFER_SIZE * sizeof(char));
                        //for (int i = 0; i < no_of_bytes; i++)
                              //buffer[i] = read_addr[i];
                        //memcpy(shared_char, buffer, no_of_bytes);
                        strcpy(message, "SUCCESS#");
                        bytesWritten = write(respPipefd, message, strlen(message));
                        if (bytesWritten < 0)
                        {
                              printf("ERROR\nwritting in the response pipe\n");
                              close(reqPipefd);
                              close(respPipefd);
                              unlink(respPipeName);
                              unlink(reqPipeName);
                              exit(1);
                        }
                        //free(buffer);
                  }
            }
      }
      free(message);
      return 0;
}
