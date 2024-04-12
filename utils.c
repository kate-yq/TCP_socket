#include "utils.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int createDirRecursive(char *path)
{
    // Attempt to create directory
    int status = mkdir(path, 0777);
    if (status == 0)
    {
        return 0; // Successfully created directory
    }

    // If mkdir failed, check if it's because the directory already exists
    if (errno == EEXIST)
    {
        return 0; // Directory already exists
    }

    // If mkdir failed for another reason, try to create the parent directory
    char *path_copy = strdup(path);
    char *last_slash = strrchr(path_copy, '/');

    if (last_slash == NULL)
    {
        free(path_copy);
        return -1; // Failed to create directory and no parent directory found
    }

    // Remove the last part of the path
    *last_slash = '\0';

    // Recursively attempt to create the parent directory
    status = createDirRecursive(path_copy);

    // Free memory for path_copy
    free(path_copy);

    if (status != 0)
    {
        return status; // Propagate the error if creating parent directory failed
    }

    // Try to create the directory again
    return mkdir(path, 0777);
}

int createDir(char *path)
{
    // remove file name and create dir
    char *dir = strdup(path);
    char *last_slash = strrchr(dir, '/');

    if (last_slash == NULL)
    {
        free(dir);
        return -1; // Failed to create directory and no parent directory found
    }

    // Remove the last part of the path
    *last_slash = '\0';
    // Recursively attempt to create the parent directory
    return createDirRecursive(dir);
}
