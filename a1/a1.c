#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 256
#define VALID_SECTION_TYPES_SIZE 7

typedef struct
{
    char sect_name[8];
    unsigned char sect_type;
    int sect_offset;
    int sect_size;
} section_header;

typedef struct
{
    char magic[4];
    short header_size;
    unsigned char version;
    unsigned char number_of_sections;
    section_header *section_headers;
} header;

int string_ends_with(const char *str, const char *suffix)
{
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);

    return (str_len >= suffix_len) &&
           (0 == strcmp(str + (str_len - suffix_len), suffix));
}

int check_path(char *dirName)
{
    // Check if realy a directory
    struct stat fileMetadata;
    if (stat(dirName, &fileMetadata) < 0)
    {
        printf("%s", "ERROR\ngetting info about the file\n");
        return -1;
    }
    if (!S_ISDIR(fileMetadata.st_mode))
    {
        printf("%s", "ERROR\nnot a directory\n");
        return -1;
    }
    return 1;
}
int check_directory(char *dirName)
{
    struct stat fileMetadata;

    if (stat(dirName, &fileMetadata) < 0)
        return -1;

    if (!S_ISDIR(fileMetadata.st_mode))
        return -1;

    return 1;
}

void list_directory(char *dirName)
{
    DIR *dir;
    struct dirent *dirEntry;

    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("%s", "ERROR\nerror opening directory\n");
        exit(4);
    }

    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            printf("%s/%s", dirName, dirEntry->d_name);
            printf("\n");
        }
    }

    closedir(dir);
}
void list_directory_filtered_name(char *dirName, char *filter)
{
    DIR *dir;
    struct dirent *dirEntry;

    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("%s", "ERROR\nerror opening directory\n");
        exit(4);
    }

    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0 && string_ends_with(dirEntry->d_name, filter))
        {
            printf("%s/%s", dirName, dirEntry->d_name);
            printf("\n");
        }
    }

    closedir(dir);
}
void list_directory_filtered_perm(char *dirName, char *filter)
{
    DIR *dir;
    struct dirent *dirEntry;
    struct stat fileMetadata;
    char *permision = malloc(MAX_PATH_LEN * sizeof(char));
    char *path = malloc(MAX_PATH_LEN * sizeof(char));

    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("%s", "ERROR\nerror opening directory\n");
        exit(4);
    }

    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            strcpy(path, dirName);
            strcat(path, "/");
            strcat(path, dirEntry->d_name);
            // printf("%s\n",path);

            if (stat(path, &fileMetadata) < 0)
            { // get info about a file system element (file, directory etc.)
                printf("ERROR\ngetting info about the file\n)");
                exit(2);
            }
            else
            {
                if (fileMetadata.st_mode & S_IRUSR) // check owner's READ permission (S_IRUSR is the mask 0400)
                    strcat(permision, "r");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IWUSR) // check owner's WRITE permission (S_IWUSR is the mask 0200)
                    strcat(permision, "w");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IXUSR) // check owner's EXECUTION permission (S_IXUSR is the mask 0100)
                    strcat(permision, "x");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IRGRP) // check group's READ permission (S_IRGRP is the mask 0040)
                    strcat(permision, "r");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IWGRP) // check group's WRITE permission (S_IWGRP is the mask 0020)
                    strcat(permision, "w");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IXGRP) // check group's EXECUTION permission (S_IXGRP is the mask 0010)
                    strcat(permision, "x");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IROTH) // check others' READ permission (S_IROTH is the mask 0004)
                    strcat(permision, "r");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IWOTH) // check others' WRITE permission (S_IWOTH is the mask 0002)
                    strcat(permision, "w");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IXOTH) // check others' EXECUTION permission (S_IXOTH is the mask 0001)
                    strcat(permision, "x");
                else
                {
                    strcat(permision, "-");
                }
                if (strcmp(permision, filter) == 0)
                {
                    printf("%s/%s", dirName, dirEntry->d_name);
                    printf("\n");
                }
                strcpy(permision, "");
            }
        }
    }

    free(permision);
    free(path);
    closedir(dir);
}
void list_directory_rec(char *dirName, char *prefix)
{
    DIR *dir;
    struct dirent *dirEntry;
    char *path = malloc(MAX_PATH_LEN * sizeof(char));
    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("%s", "ERROR\nerror opening directory\n");
        exit(4);
    }

    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            strcpy(path, dirName);
            strcat(path, "/");
            strcat(path, dirEntry->d_name);
            printf("%s\n", path);
            if (check_directory(path) == 1)
            {
                char *new_prefix = malloc(MAX_PATH_LEN * sizeof(char));
                strcpy(new_prefix, prefix);
                strcat(new_prefix, dirEntry->d_name);
                strcat(new_prefix, "/");
                list_directory_rec(path, new_prefix);
                free(new_prefix);
            }
        }
    }
    free(path);
    closedir(dir);
}
void list_directory_rec_filtered_name(char *dirName, char *prefix, char *filter)
{
    DIR *dir;
    struct dirent *dirEntry;
    char *path = malloc(MAX_PATH_LEN * sizeof(char));
    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("%s", "ERROR\nerror opening directory\n");
        exit(4);
    }

    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            strcpy(path, dirName);
            strcat(path, "/");
            strcat(path, dirEntry->d_name);
            if (string_ends_with(path, filter))
                printf("%s\n", path);
            if (check_directory(path) == 1)
            {
                char *new_prefix = malloc(MAX_PATH_LEN * sizeof(char));
                strcpy(new_prefix, prefix);
                strcat(new_prefix, dirEntry->d_name);
                strcat(new_prefix, "/");
                list_directory_rec_filtered_name(path, new_prefix, filter);
                free(new_prefix);
            }
        }
    }
    free(path);
    closedir(dir);
}
void list_directory_rec_filtered_perm(char *dirName, char *prefix, char *filter)
{
    DIR *dir;
    struct dirent *dirEntry;
    struct stat fileMetadata;
    char *permision = malloc(MAX_PATH_LEN * sizeof(char));
    char *path = malloc(MAX_PATH_LEN * sizeof(char));

    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("%s", "ERROR\nerror opening directory\n");
        exit(4);
    }

    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            strcpy(path, dirName);
            strcat(path, "/");
            strcat(path, dirEntry->d_name);

            if (stat(path, &fileMetadata) < 0)
            { // get info about a file system element (file, directory etc.)
                printf("ERROR\ngetting info about the file\n)");
                exit(2);
            }
            else
            {
                if (fileMetadata.st_mode & S_IRUSR) // check owner's READ permission (S_IRUSR is the mask 0400)
                    strcat(permision, "r");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IWUSR) // check owner's WRITE permission (S_IWUSR is the mask 0200)
                    strcat(permision, "w");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IXUSR) // check owner's EXECUTION permission (S_IXUSR is the mask 0100)
                    strcat(permision, "x");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IRGRP) // check group's READ permission (S_IRGRP is the mask 0040)
                    strcat(permision, "r");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IWGRP) // check group's WRITE permission (S_IWGRP is the mask 0020)
                    strcat(permision, "w");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IXGRP) // check group's EXECUTION permission (S_IXGRP is the mask 0010)
                    strcat(permision, "x");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IROTH) // check others' READ permission (S_IROTH is the mask 0004)
                    strcat(permision, "r");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IWOTH) // check others' WRITE permission (S_IWOTH is the mask 0002)
                    strcat(permision, "w");
                else
                    strcat(permision, "-");

                if (fileMetadata.st_mode & S_IXOTH) // check others' EXECUTION permission (S_IXOTH is the mask 0001)
                    strcat(permision, "x");
                else
                {
                    strcat(permision, "-");
                }
            }
            if (strcmp(permision, filter) == 0)
            {
                printf("%s\n", path);
            }
            strcpy(permision, "");
            if (check_directory(path) == 1)
            {
                char *new_prefix = malloc(MAX_PATH_LEN * sizeof(char));
                strcpy(new_prefix, prefix);
                strcat(new_prefix, dirEntry->d_name);
                strcat(new_prefix, "/");
                list_directory_rec_filtered_name(path, new_prefix, filter);
                free(new_prefix);
            }
        }
    }

    free(permision);
    free(path);
    closedir(dir);
}
int is_valid_section_type(int section_type)
{
    int valid_section_types[7] = {72, 56, 64, 40, 37, 48, 41};
    int i;
    for (i = 0; i < VALID_SECTION_TYPES_SIZE; i++)
    {
        if (section_type == valid_section_types[i])
        {
            return 1;
        }
    }
    return 0;
}
void parse_sf_file(char *file_path)
{
    int fd, i, bytesRead;
    header *file_header = malloc(sizeof(header));
    fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        printf("ERROR\nCouldn't open file\n");
        free(file_header);
        return;
    }
    // READ magic
    bytesRead = read(fd, &file_header->magic, sizeof(file_header->magic));
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    else
    {
        if (strcmp(file_header->magic, "JUoc") != 0)
        {
            printf("ERROR\nwrong magic\n");
            free(file_header);
            return;
        }
    }
    // READ header_size
    bytesRead = read(fd, &file_header->header_size, sizeof(file_header->header_size));
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    // READ version
    bytesRead = read(fd, &file_header->version, sizeof(file_header->version));
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    else
    {
        if (file_header->version < 82 || file_header->version > 172)
        {
            printf("ERROR\nwrong version\n");
            free(file_header);
            return;
        }
    }

    // READ number_of_sections
    bytesRead = read(fd, &file_header->number_of_sections, sizeof(file_header->number_of_sections));

    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    else
    {
        if (!(file_header->number_of_sections >= 8 && file_header->number_of_sections <= 10))
        {
            printf("ERROR\nwrong sect_nr\n");
            free(file_header);
            return;
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
            return;
        }
        // READ sect_type
        bytesRead = read(fd, &file_header->section_headers[i].sect_type, sizeof(file_header->section_headers->sect_type));
        if (bytesRead < 0)
        {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header->section_headers);
            free(file_header);
            return;
        }
        else
        {
            if (is_valid_section_type(file_header->section_headers[i].sect_type) != 1)
            {
                printf("ERROR\nwrong sect_types\n");
                free(file_header->section_headers);
                free(file_header);
                return;
            }
        }
        // READ sect_offset
        bytesRead = read(fd, &file_header->section_headers[i].sect_offset, sizeof(file_header->section_headers->sect_offset));
        if (bytesRead < 0)
        {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header->section_headers);
            free(file_header);
            return;
        }
        // READ sect_size
        bytesRead = read(fd, &file_header->section_headers[i].sect_size, sizeof(file_header->section_headers->sect_size));
        if (bytesRead < 0)
        {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header->section_headers);
            free(file_header);
            return;
        }
    }
    printf("SUCCESS\n");
    printf("version=%i\n", file_header->version);
    printf("nr_sections=%i\n", file_header->number_of_sections);
    for (int i = 0; i < file_header->number_of_sections; i++)
    {
        printf("section%i: %s %i %i", i + 1, file_header->section_headers[i].sect_name, file_header->section_headers[i].sect_type, file_header->section_headers[i].sect_size);
        printf("\n");
    }

    free(file_header->section_headers);
    free(file_header);
}
void extract(char *file_path, int sect_nr, int line_nr)
{
    int fd, i, bytesRead;
    header *file_header = malloc(sizeof(header));
    fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        printf("ERROR\nCouldn't open file\n");
        free(file_header);
        return;
    }
    // READ magic
    bytesRead = read(fd, &file_header->magic, sizeof(file_header->magic));
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    else
    {
        if (strcmp(file_header->magic, "JUoc") != 0)
        {
            printf("ERROR\ninvalid file\n");
            free(file_header);
            return;
        }
    }
    // READ header_size
    bytesRead = read(fd, &file_header->header_size, sizeof(file_header->header_size));
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    // READ version
    bytesRead = read(fd, &file_header->version, sizeof(file_header->version));
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    else
    {
        if (file_header->version < 82 || file_header->version > 172)
        {
            printf("ERROR\ninvalid file\n");
            free(file_header);
            return;
        }
    }
    // READ number_of_sections
    bytesRead = read(fd, &file_header->number_of_sections, sizeof(file_header->number_of_sections));
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(file_header);
        return;
    }
    else
    {
        if (!(file_header->number_of_sections >= 8 && file_header->number_of_sections <= 10))
        {
            printf("ERROR\ninvalid file\n");
            free(file_header);
            return;
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
            return;
        }
        // READ sect_type
        bytesRead = read(fd, &file_header->section_headers[i].sect_type, sizeof(file_header->section_headers->sect_type));
        if (bytesRead < 0)
        {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header->section_headers);
            free(file_header);
            return;
        }
        else
        {
            if (is_valid_section_type(file_header->section_headers[i].sect_type) != 1)
            {
                printf("ERROR\ninvalid file\n");
                free(file_header->section_headers);
                free(file_header);
                return;
            }
        }
        // READ sect_offset
        bytesRead = read(fd, &file_header->section_headers[i].sect_offset, sizeof(file_header->section_headers->sect_offset));
        if (bytesRead < 0)
        {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header->section_headers);
            free(file_header);
            return;
        }
        // READ sect_size
        bytesRead = read(fd, &file_header->section_headers[i].sect_size, sizeof(file_header->section_headers->sect_size));
        if (bytesRead < 0)
        {
            printf("ERROR\nCouldn't read from the file\n");
            free(file_header->section_headers);
            free(file_header);
            return;
        }
    }

    if (file_header->number_of_sections < sect_nr)
    {
        printf("ERROR\ninvalid section\n");
        free(file_header->section_headers);
        free(file_header);
        return;
    }
    char *buffer = malloc(file_header->section_headers[sect_nr - 1].sect_size * sizeof(char));
    lseek(fd, 0, SEEK_SET);
    lseek(fd, file_header->section_headers[sect_nr - 1].sect_offset, SEEK_SET);

    bytesRead = read(fd, buffer, file_header->section_headers[sect_nr - 1].sect_size);
    if (bytesRead < 0)
    {
        printf("ERROR\nCouldn't read from the file\n");
        free(buffer);
        free(file_header->section_headers);
        free(file_header);
        return;
    }

    int nr_lines = 1, valid = 0;
    char *token;
    const char s[2] = "\n";
    token = strtok(buffer, s);

    while (token != NULL)
    {
        if (nr_lines == line_nr)
        {
            printf("SUCCESS\n");
            printf("%s\n", token);
            valid = 1;
        }
        nr_lines++;
        token = strtok(NULL, s);
    }
    if (valid == 0)
        printf("ERROR\ninvalid line\n");

    free(buffer);
    free(file_header->section_headers);
    free(file_header);
}
int check_SF_15(char *file_path)
{
    int fd, i, bytesRead;
    header *file_header = malloc(sizeof(header));
    fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        close(fd);
        free(file_header);
        return 0;
    }
    // READ magic
    bytesRead = read(fd, &file_header->magic, sizeof(file_header->magic));
    if (bytesRead < 0)
    {
        close(fd);
        free(file_header);
        return 0;
    }
    else
    {
        if (file_header->magic[0] != 'J' && file_header->magic[1] != 'U' && file_header->magic[2] != 'o' && file_header->magic[3] != 'c')
        {
            close(fd);
            free(file_header);
            return 0;
        }
    }
    // READ header_size
    bytesRead = read(fd, &file_header->header_size, sizeof(file_header->header_size));
    if (bytesRead < 0)
    {
        close(fd);
        free(file_header);
        return 0;
    }
    // READ version
    bytesRead = read(fd, &file_header->version, sizeof(file_header->version));
    if (bytesRead < 0)
    {
        close(fd);
        free(file_header);
        return 0;
    }
    else
    {
        if (file_header->version < 82 || file_header->version > 172)
        {
            close(fd);
            free(file_header);
            return 0;
        }
    }

    // READ number_of_sections
    bytesRead = read(fd, &file_header->number_of_sections, sizeof(file_header->number_of_sections));

    if (bytesRead < 0)
    {
        close(fd);
        free(file_header);
        return 0;
    }
    else
    {
        if (!(file_header->number_of_sections >= 8 && file_header->number_of_sections <= 10))
        {
            close(fd);
            free(file_header);
            return 0;
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
            close(fd);
            free(file_header->section_headers);
            free(file_header);
            return 0;
        }
        // READ sect_type
        bytesRead = read(fd, &file_header->section_headers[i].sect_type, sizeof(file_header->section_headers->sect_type));
        if (bytesRead < 0)
        {
            close(fd);
            free(file_header->section_headers);
            free(file_header);
            return 0;
        }
        else
        {
            if (is_valid_section_type(file_header->section_headers[i].sect_type) != 1)
            {
                close(fd);
                free(file_header->section_headers);
                free(file_header);
                return 0;
            }
        }
        // READ sect_offset
        bytesRead = read(fd, &file_header->section_headers[i].sect_offset, sizeof(file_header->section_headers->sect_offset));
        if (bytesRead < 0)
        {
            close(fd);
            free(file_header->section_headers);
            free(file_header);
            return 0;
        }
        // READ sect_size
        bytesRead = read(fd, &file_header->section_headers[i].sect_size, sizeof(file_header->section_headers->sect_size));
        if (bytesRead < 0)
        {
            close(fd);
            free(file_header->section_headers);
            free(file_header);
            return 0;
        }
    }
    for (int i = 0; i < file_header->number_of_sections; i++)
    {
        char *buffer = malloc(file_header->section_headers[i].sect_size * sizeof(char));
        lseek(fd, 0, SEEK_SET);
        lseek(fd, file_header->section_headers[i].sect_offset, SEEK_SET);

        bytesRead = read(fd, buffer, file_header->section_headers[i].sect_size);
        if (bytesRead < 0)
        {
            close(fd);
            free(buffer);
            free(file_header->section_headers);
            free(file_header);
            return 0;
        }

        int nr_lines = 1;
        char *token;
        const char s[2] = "\n";
        token = strtok(buffer, s);

        while (token != NULL)
        {
            nr_lines++;
            token = strtok(NULL, s);
        }
        if (nr_lines >= 15)
        {
            free(file_header->section_headers);
            free(file_header);
            free(buffer);
            close(fd);
            return 1;
        }
        free(buffer);
    }
    close(fd);
    free(file_header->section_headers);
    free(file_header);
    return 0;
}
void list_directory_rec_SF(char *dirName, char *prefix)
{
    DIR *dir;
    struct dirent *dirEntry;
    char *path = malloc(MAX_PATH_LEN * sizeof(char));
    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("%s", "ERROR\nerror opening directory\n");
        exit(4);
    }

    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            strcpy(path, dirName);
            strcat(path, "/");
            strcat(path, dirEntry->d_name);
            if (check_directory(path) == 1)
            {
                char *new_prefix = malloc(MAX_PATH_LEN * sizeof(char));
                strcpy(new_prefix, prefix);
                strcat(new_prefix, dirEntry->d_name);
                strcat(new_prefix, "/");
                list_directory_rec_SF(path, new_prefix);
                free(new_prefix);
            }
            else
            {
                if (check_SF_15(path) == 1)
                {
                    printf("%s\n", path);
                }
            }
        }
    }
    free(path);
    closedir(dir);
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("%s", "81069\n");
        }
        else if (strcmp(argv[1], "list") == 0)
        {
            char *path = malloc(MAX_PATH_LEN * sizeof(char));
            char *op_name = malloc(MAX_PATH_LEN * sizeof(char));
            char *op_perm = malloc(MAX_PATH_LEN * sizeof(char));
            if (argc > 2)
            {
                if (strcmp(argv[2], "recursive") == 0)
                {
                    if (argc >= 4)
                    {
                        if (argc == 4) // list recursive path
                        {
                            strncpy(path, argv[3] + 5, strlen(argv[3]));
                            if (check_path(path) == 1)
                            {
                                char *prefix = "";
                                printf("%s", "SUCCESS\n");
                                list_directory_rec(path, prefix);
                            }
                        }
                        else
                        {
                            char *ptr = strstr(argv[3], "path");
                            if (ptr != NULL) // list recursive path operation
                            {
                                strncpy(path, argv[3] + 5, strlen(argv[3]));
                                if (check_path(path) == 1)
                                {
                                    printf("%s", "SUCCESS\n");
                                    if (strstr(argv[4], "name_ends_with") != NULL) // name_ends_with filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_name, argv[4] + 15, strlen(argv[4]));
                                        list_directory_rec_filtered_name(path, prefix, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_perm, argv[4] + 12, strlen(argv[4]));
                                        list_directory_rec_filtered_perm(path, prefix, op_perm);
                                    }
                                }
                            }
                            else // list recursive operation path
                            {
                                strncpy(path, argv[4] + 5, strlen(argv[4]));
                                if (check_path(path) == 1)
                                {
                                    printf("%s", "SUCCESS\n");
                                    if (strstr(argv[3], "name_ends_with") != NULL) // name_ends_with filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_name, argv[3] + 15, strlen(argv[3]));
                                        list_directory_rec_filtered_name(path, prefix, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_perm, argv[3] + 12, strlen(argv[3]));
                                        list_directory_rec_filtered_perm(path, prefix, op_perm);
                                    }
                                }
                            }
                        }
                    }
                    else
                        printf("%s", "ERROR\nmissing directory path\n");
                }
                else if (strcmp(argv[argc - 1], "recursive") == 0)
                {
                    if (argc >= 4)
                    {

                        if (argc == 4)
                        {
                            // list path recursive
                            strncpy(path, argv[2] + 5, strlen(argv[2]));
                            if (check_path(path) == 1)
                            {
                                char *prefix = "";
                                printf("%s", "SUCCESS\n");
                                list_directory_rec(path, prefix);
                            }
                        }

                        else
                        {
                            char *ptr = strstr(argv[2], "path");
                            if (ptr != NULL) // list path operation recursive
                            {
                                strncpy(path, argv[2] + 5, strlen(argv[2]));
                                if (check_path(path) == 1)
                                {
                                    printf("%s", "SUCCESS\n");
                                    if (strstr(argv[3], "name_ends_with") != NULL) // name_ends_with filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_name, argv[3] + 15, strlen(argv[3]));
                                        list_directory_rec_filtered_name(path, prefix, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_perm, argv[3] + 12, strlen(argv[3]));
                                        list_directory_rec_filtered_perm(path, prefix, op_perm);
                                    }
                                }
                            }
                            else // list operation path recursive
                            {
                                strncpy(path, argv[3] + 5, strlen(argv[3]));
                                if (check_path(path) == 1)
                                {
                                    printf("%s", "SUCCESS\n");
                                    if (strstr(argv[2], "name_ends_with") != NULL) // name_ends_with filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_name, argv[2] + 15, strlen(argv[2]));
                                        list_directory_rec_filtered_name(path, prefix, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        char *prefix = "";
                                        strncpy(op_perm, argv[2] + 12, strlen(argv[2]));
                                        list_directory_rec_filtered_perm(path, prefix, op_perm);
                                    }
                                }
                            }
                        }
                    }
                    else
                        printf("%s", "ERROR\nmissing directory path\n");
                }
                else
                {

                    if (argc == 3) // list path
                    {
                        strncpy(path, argv[2] + 5, strlen(argv[2]));
                        if (check_path(path) == 1)
                        {
                            printf("%s", "SUCCESS\n");
                            list_directory(path);
                        }
                    }

                    else
                    {
                        char *ptr = strstr(argv[2], "path");
                        if (ptr != NULL) // list path operatiom
                        {
                            strncpy(path, argv[2] + 5, strlen(argv[2]));
                            if (check_path(path) == 1)
                            {
                                printf("%s", "SUCCESS\n");
                                if (strstr(argv[3], "name_ends_with") != NULL) // name_ends_with filter
                                {
                                    strncpy(op_name, argv[3] + 15, strlen(argv[3]));
                                    list_directory_filtered_name(path, op_name);
                                }
                                else // permissions filter
                                {
                                    strncpy(op_perm, argv[3] + 12, strlen(argv[3]));
                                    list_directory_filtered_perm(path, op_perm);
                                }
                            }
                        }
                        else // list operation path
                        {
                            strncpy(path, argv[3] + 5, strlen(argv[3]));
                            if (check_path(path) == 1)
                            {
                                printf("%s", "SUCCESS\n");
                                if (strstr(argv[2], "name_ends_with") != NULL) // name_ends_with filter
                                {
                                    strncpy(op_name, argv[2] + 15, strlen(argv[2]));
                                    list_directory_filtered_name(path, op_name);
                                }
                                else // permissions filter
                                {
                                    strncpy(op_perm, argv[2] + 12, strlen(argv[2]));
                                    list_directory_filtered_perm(path, op_perm);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                printf("%s", "ERROR\nnot enough parameters\n");
            }
            free(path);
            free(op_name);
            free(op_perm);
        }
        else if (strcmp(argv[1], "parse") == 0 || strcmp(argv[2], "parse") == 0)
        {
            char *path = malloc(MAX_PATH_LEN * sizeof(char));

            if (strcmp(argv[1], "parse") == 0)
            {
                strncpy(path, argv[2] + 5, strlen(argv[2]));
            }
            else if (strcmp(argv[2], "parse") == 0)
            {
                strncpy(path, argv[1] + 5, strlen(argv[1]));
            }
            parse_sf_file(path);
            free(path);
        }
        else if (strcmp(argv[1], "extract") == 0)
        {
            char *path = malloc(MAX_PATH_LEN * sizeof(char));
            char *sect_nr = malloc(MAX_PATH_LEN * sizeof(char));
            char *line_nr = malloc(MAX_PATH_LEN * sizeof(char));

            strncpy(path, argv[2] + 5, strlen(argv[2]));
            strncpy(sect_nr, argv[3] + 8, strlen(argv[3]));
            strncpy(line_nr, argv[4] + 5, strlen(argv[4]));

            extract(path, atoi(sect_nr), atoi(line_nr));

            free(path);
            free(sect_nr);
            free(line_nr);
        }
        else if (strcmp(argv[1], "findall") == 0)
        {
            char *path = malloc(MAX_PATH_LEN * sizeof(char));
            strncpy(path, argv[2] + 5, strlen(argv[2]));

            char *prefix = "";
            printf("%s", "SUCCESS\n");
            list_directory_rec_SF(path, prefix);

            free(path);
        }
    }
    return 0;
}