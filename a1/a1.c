#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 256
#define MAGIC "JUoc"
#define MIN_VERSION 82
#define MAX_VERSION 172
#define MIN_SECTIONS 8
#define MAX_SECTIONS 10
#define VALID_SECTION_TYPES_SIZE 7
#define VALID_SECTION_TYPES        \
    {                              \
        72, 56, 64, 40, 37, 48, 41 \
    }

typedef struct
{
    char name[7];
    int type;
    int offset;
    int size;
} section_t;

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
    int valid_section_types[] = VALID_SECTION_TYPES;
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
    int fd, i;
    char magic[5];
    int version, sections_nr, header_size;
    section_t sections[MAX_SECTIONS];

    fd = open(file_path, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR\nFailed to open file: %s\n", file_path);
        return;
    }

    read(fd, magic, 4);
    magic[4] = '\0';
    if (strcmp(magic, MAGIC) != 0)
    {
        printf("ERROR\nwrong magic\n");
        close(fd);
        return;
    }
    read(fd, &header_size, 2);
    read(fd, &version, 1);
    if (!(version >= MIN_VERSION && version <= MAX_VERSION))
    {
        printf("ERROR\nwrong version\n");
        close(fd);
        return;
    }

    read(fd, &sections_nr, 1);
    if (!(sections_nr >= MIN_SECTIONS && sections_nr <= MAX_SECTIONS))
    {
        printf("ERROR\nwrong section numberr\n");
        close(fd);
        return;
    }
    for (i = 0; i < sections_nr; i++)
    {
        read(fd, sections[i].name, 7);
        sections[i].name[7] = '\0';
        read(fd, &sections[i].type, 1);
        read(fd, &sections[i].offset, 4);
        read(fd, &sections[i].size, 4);
        if (!is_valid_section_type(sections[i].type))
        {
            printf("ERROR\nwrong section types\n");
            close(fd);
            return;
        }
    }
    close(fd);

    printf("SUCCESS\n");
    printf("version=%d\n", version);
    printf("nr_sections=%d\n", sections_nr);
    for (i = 0; i < sections_nr; i++)
    {
        printf("section%d: %s %d %d\n", i + 1, sections[i].name, sections[i].type, sections[i].size);
    }
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
    }
    return 0;
}