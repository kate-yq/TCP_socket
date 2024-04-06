#ifndef COMMAND_H
#define COMMAND_H

char *commandToString(char **args, int size);
char **stringToCommand(char *str);
void freeCharPtr(char **args);

#endif /* COMMAND_H */