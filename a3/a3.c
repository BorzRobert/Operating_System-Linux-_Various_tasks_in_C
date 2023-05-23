#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

#define MAX_BUFFER_SIZE 1024

typedef struct
{
      char sect_name[8];
      unsigned char sect_type;
      int sect_offset;
      int sect_size;
} section_header;
typedef struct
{
      char magic[5];
      short header_size;
      unsigned char version;
      unsigned char number_of_sections;
      section_header *section_headers;
} header;
int is_valid_section_type(int section_type)
{
      int valid_section_types[7] = {72, 56, 64, 40, 37, 48, 41};
      int i;
      for (i = 0; i < 7; i++)
      {
            if (section_type == valid_section_types[i])
            {
                  return 1;
            }
      }
      return 0;
}
int extract(int fd, int sect_nr)
{
      int i, bytesRead;
      header *file_header = malloc(sizeof(header));
      if (fd < 0)
      {
            printf("ERROR\nCouldn't open file\n");
            free(file_header);
            return -1;
      }
      // READ magic
      bytesRead = read(fd, &file_header->magic, 4);
      if (bytesRead < 0)
      {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header);
            return -1;
      }
      else
      {
            file_header->magic[4] = '\0';
            if (strcmp(file_header->magic, "JUoc") != 0)
            {
                  printf("ERROR\ninvalid file\n");
                  free(file_header);
                  return -1;
            }
      }
      // READ header_size
      bytesRead = read(fd, &file_header->header_size, sizeof(file_header->header_size));
      if (bytesRead < 0)
      {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header);
            return -1;
      }
      // READ version
      bytesRead = read(fd, &file_header->version, sizeof(file_header->version));
      if (bytesRead < 0)
      {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header);
            return -1;
      }
      else
      {
            if (file_header->version < 82 || file_header->version > 172)
            {
                  printf("ERROR\ninvalid file\n");
                  free(file_header);
                  return -1;
            }
      }
      // READ number_of_sections
      bytesRead = read(fd, &file_header->number_of_sections, sizeof(file_header->number_of_sections));
      if (bytesRead < 0)
      {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header);
            return -1;
      }
      else
      {
            if (!(file_header->number_of_sections >= 8 && file_header->number_of_sections <= 10))
            {
                  printf("ERROR\ninvalid file\n");
                  free(file_header);
                  return -1;
            }
      }

      // READ sections headers
      file_header->section_headers = malloc(file_header->number_of_sections * sizeof(section_header));
      for (i = 0; i < file_header->number_of_sections; i++)
      {
            // READ sect_name
            bytesRead = read(fd, &file_header->section_headers[i].sect_name, 7);
            if (bytesRead < 0)
            {
                  printf("ERROR\nCouldn't read from the file\n");
                  free(file_header->section_headers);
                  free(file_header);
                  return -1;
            }
            // READ sect_type
            bytesRead = read(fd, &file_header->section_headers[i].sect_type, sizeof(file_header->section_headers->sect_type));
            if (bytesRead < 0)
            {
                  printf("ERROR\nCouldn't read from the file\n");
                  free(file_header->section_headers);
                  free(file_header);
                  return -1;
            }
            else
            {
                  if (is_valid_section_type(file_header->section_headers[i].sect_type) != 1)
                  {
                        printf("ERROR\ninvalid file\n");
                        free(file_header->section_headers);
                        free(file_header);
                        return -1;
                  }
            }
            // READ sect_offset
            bytesRead = read(fd, &file_header->section_headers[i].sect_offset, sizeof(file_header->section_headers->sect_offset));
            if (bytesRead < 0)
            {
                  printf("ERROR\nCouldn't read from the file\n");
                  free(file_header->section_headers);
                  free(file_header);
                  return -1;
            }
            return file_header->section_headers[i].sect_offset;
            // READ sect_size
            bytesRead = read(fd, &file_header->section_headers[i].sect_size, sizeof(file_header->section_headers->sect_size));
            if (bytesRead < 0)
            {
                  printf("ERROR\nCouldn't read from the file\n");
                  free(file_header->section_headers);
                  free(file_header);
                  return -1;
            }
      }
      if (file_header->number_of_sections < sect_nr)
      {
            printf("ERROR\ninvalid section\n");
            free(file_header->section_headers);
            free(file_header);
            return -1;
      }
      free(file_header->section_headers);
      free(file_header);
      return file_header->section_headers[sect_nr - 1].sect_offset;
}

int main()
{printf("programul meu merge\n");
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
            else if (strcmp(request, "READ_FROM_FILE_OFFSET") == 0) // Read from file offset request
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
                        char *read_addr = shared_char_file + offset;
                        char *buffer = malloc(MAX_BUFFER_SIZE * sizeof(char));
                        for (int i = 0; i < no_of_bytes; i++)
                              buffer[i] = read_addr[i];
                        memcpy(shared_char, buffer, no_of_bytes);
                        free(buffer);
                  }
            }
            else if (strcmp(request, "READ_FROM_FILE_SECTION")==0) // Read from file section request
            {
                  unsigned int section_no, offset, no_of_bytes;
                  strcpy(message, "READ_FROM_FILE_SECTION#");
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
                  bytes_read = read(reqPipefd, &section_no, sizeof(section_no));
                  if (bytes_read < 0)
                  {
                        printf("ERROR\nreading from the request pipe\n");
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
                  if (section_no < 8 || section_no > 10)
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
                        int sf_offset;
                        if (extract(fd, section_no) != -1)
                              sf_offset = extract(fd, section_no);
                        else
                        {
                              printf("ERROR\ncouldn't find sf_offset");
                              exit(1);
                        }
                        char *read_addr = shared_char_file + offset + sf_offset;

                        char *buffer = malloc(MAX_BUFFER_SIZE * sizeof(char));
                        for (int i = 0; i < no_of_bytes; i++)
                              buffer[i] = read_addr[i];
                        memcpy(shared_char, buffer, no_of_bytes);
                        free(buffer);
                  }
            }
      }
      free(message);
      return 0;
}
