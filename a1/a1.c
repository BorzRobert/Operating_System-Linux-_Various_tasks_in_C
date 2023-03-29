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
                        printf("%s", "SUCCESS\n");
                        // TODO: If the “recursive” option is also specified, your program should traverse the
                        // entire sub-tree starting from the given directory, entering recursively in all the
                        // sub-folders, sub-sub-folders and so on. Found element names must also contain
                        // their path relative to the given directory. The two boxes below illustrate the
                        // way the program could be run recursively and a possible output for that case,
                        // respectively.
                        if (argc == 4)
                            printf("%s", "list recursive path\n");
                        else
                        {
                            char *ptr = strstr(argv[3], "path");
                            if (ptr != NULL)
                            {
                                printf("%s", "list recursive path operation\n");
                            }
                            else
                            {
                                printf("%s", "list recursive operation path\n");
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

                        printf("%s", "SUCCESS\n");

                        if (argc == 4)
                            printf("%s", "list path recursive\n");
                        else
                        {
                            char *ptr = strstr(argv[2], "path");
                            if (ptr != NULL)
                            {
                                printf("%s", "list path operation recursive\n");
                            }
                            else
                            {
                                printf("%s", "list operation path recursive\n");
                            }
                        }
                    }
                    else
                        printf("%s", "ERROR\nmissing directory path\n");
                }
                else
                {

                    if (argc == 3)
                    {
                        strncpy(path, argv[2] + 5, strlen(argv[2]));
                        // printf("%s", "list  path\n");
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
        }
    }
    return 0;
}