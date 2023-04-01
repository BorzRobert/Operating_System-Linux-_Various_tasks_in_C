#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 256

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
    { // get info
        printf("%s", "ERROR\ngetting info about the file\n");
        return -1;
    }
    if (!S_ISDIR(fileMetadata.st_mode))
    { // it is a directory
        printf("%s", "ERROR\nnot a directory\n");
        return -1;
    }
    return 1;
}
int check_directory(char *dirName)
{
    // Check if realy a directory
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
void list_directory_rec(char *dirName, int *dirPrinted)
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
    if (!(*dirPrinted))
    {
        printf("%s\n", dirName);
        *dirPrinted = 1;
    }
    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            strcat(path, dirName);
            strcat(path, "/");
            strcat(path, dirEntry->d_name);
            if (check_directory(path) == 1)
            {
                list_directory_rec(path, dirPrinted);
            }
            printf("%s/%s", dirName, dirEntry->d_name);
            printf("\n");
        }
    }

    free(path);
    closedir(dir);
}
void list_directory_rec_filtered_name(char *dirName, int *dirPrinted, char *filter)
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
    if (!(*dirPrinted) && string_ends_with(dirName, filter))
    {
        printf("%s\n", dirName);
        *dirPrinted = 1;
    }
    // iterate the directory contents
    while ((dirEntry = readdir(dir)) != 0)
    {
        // build the complete path to the element in the directory
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            strcat(path, dirName);
            strcat(path, "/");
            strcat(path, dirEntry->d_name);
            if (check_directory(path) == 1)
            {
                *dirPrinted = 1;
                list_directory_rec_filtered_name(path, dirPrinted, filter);
            }
            if (string_ends_with(dirEntry->d_name, filter))
            {
                printf("%s/%s", dirName, dirEntry->d_name);
                printf("\n");
            }
        }
    }

    free(path);
    closedir(dir);
}
void list_directory_rec_filtered_perm(char *dirName, int *dirPrinted, char *filter)
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
            if (check_directory(path) == 1)
            {
                *dirPrinted = 1;
                list_directory_rec_filtered_perm(path, dirPrinted, filter);
            }
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
                if (!(*dirPrinted) && strcmp(permision, filter) == 0)
                {
                    printf("%s\n", dirName);
                    *dirPrinted = 1;
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
            int dirPrinted = 0;
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
                                printf("%s", "SUCCESS\n");
                                list_directory_rec(path, &dirPrinted);
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
                                        strncpy(op_name, argv[4] + 15, strlen(argv[4]));
                                        list_directory_rec_filtered_name(path, &dirPrinted, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        strncpy(op_perm, argv[4] + 12, strlen(argv[4]));
                                        list_directory_rec_filtered_perm(path, &dirPrinted, op_perm);
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
                                        strncpy(op_name, argv[3] + 15, strlen(argv[3]));
                                        list_directory_rec_filtered_name(path, &dirPrinted, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        strncpy(op_perm, argv[3] + 12, strlen(argv[3]));
                                        list_directory_rec_filtered_perm(path, &dirPrinted, op_perm);
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
                                printf("%s", "SUCCESS\n");
                                list_directory_rec(path, &dirPrinted);
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
                                        strncpy(op_name, argv[3] + 15, strlen(argv[3]));
                                        list_directory_rec_filtered_name(path, &dirPrinted, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        strncpy(op_perm, argv[3] + 12, strlen(argv[3]));
                                        list_directory_rec_filtered_perm(path, &dirPrinted, op_perm);
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
                                        strncpy(op_name, argv[2] + 15, strlen(argv[2]));
                                        list_directory_rec_filtered_name(path, &dirPrinted, op_name);
                                    }
                                    else // permissions filter
                                    {
                                        strncpy(op_perm, argv[2] + 12, strlen(argv[2]));
                                        list_directory_rec_filtered_perm(path, &dirPrinted, op_perm);
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
            int fd;
            char buffer[100];
            char *path = malloc(MAX_PATH_LEN * sizeof(char));
            if (strcmp(argv[1], "parse") == 0)
                strncpy(path, argv[2] + 5, strlen(argv[2]));

            else if (strcmp(argv[2], "parse") == 0)
                strncpy(path, argv[1] + 5, strlen(argv[1]));

            fd=open(path,O_RDONLY);
            if(fd<0)
            printf("ERROR\nCould not open file\n");

            if(read(fd,buffer,4)<0)
            printf("ERROR\nCould not read file\n");

            for (int i = 0; i <4; i++)
            {
                printf("%c",buffer[i]);
            }

                free(path);
        }
    }
    return 0;
}