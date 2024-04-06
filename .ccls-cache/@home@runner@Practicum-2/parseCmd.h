#ifndef PARSECMD_H
#define PARSECMD_H

char *commandToString(char **args, int size);
char **stringToCommand(char *str);
void freeCharPtr(char **args);

#endif /* PARSECMD_H */