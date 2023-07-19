#include <stdio.h>
#include <string.h>
#include "DataTypes.h"
#define MAX_LINE 82
#define LONG_LABEL -1
#define SPACE_IN_LABEL -2
#define USED_LABEL -3
#define WRONG_LABEL_FORMAT -4
#define TOO_MANY_COMMAS -5
#define INVALID_PARAMS -6
#define LONG_LINE -7
#define NO_DATA -8
#define STRING_IN_DATA -9
#define DOUBLE_SIGNS -10
#define SPACE_IN_PARAMS -11
#define NOT_AN_INT -12
#define MISSING_PARENTHESIS -13
#define TOO_MANY_PARENTHESIS -14
#define NOT_ENOUGH_PARAMETERS -15
#define TOO_MANY_PARAMETERS -16
#define UNKNOWN_COMMAND -17
#define INVALID_ADDRESSING -18
#define LABEL_DOESNT_EXIST -19

int get_word(char *word,char *command);
int isEmpty(char* s);
void printMem(Inst **instMem,Data **DataMem,int IC, int DC,char *filename);
int convertTo2(int x);
void freeAll(Inst **instMem,Data **dataMem, Label **labelTable);
void errorPrinter(int line, int errorType);
Inst* allInst(Inst **instrMem,int newSize);
