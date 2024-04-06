#ifndef PARSEHDR_H
#define PARSEHDR_H

// Function to convert permission to a string
// could add more info later
char *headerToString(int permission);

// get the permission of the file
// 0 if read only
// 1 if read and write
// -1 if not exist
int getPermission(char *dir);

#endif /* PARSEHDR_H*/