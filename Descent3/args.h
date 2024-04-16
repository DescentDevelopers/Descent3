#ifndef ARGS_H
#define ARGS_H

#define MAX_ARGS			30
#define MAX_CHARS_PER_ARG	100
extern char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG];

// Gathers all arguments
void GatherArgs (char *str);
void GatherArgs (char **argv);

// Returns index of argument sought, or 0 if not found
int FindArg (char *which);
int FindArgChar(char *which, char singleCharArg);

const char* GetArg( int index );

#endif


