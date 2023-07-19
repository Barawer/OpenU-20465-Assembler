#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct Macro Macro;
typedef struct MacroList MacroList;

typedef struct Macro{
	char* name;
	char** lines;
    int totLines;
}Macro;

struct MacroList{
    Macro *head;
    MacroList *nextMacro;
};

int runPreProcess(FILE* src, FILE* dest);

int checkMacroExist(char *mcr, Macro *mcrList);