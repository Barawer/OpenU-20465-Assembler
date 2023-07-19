#include <stdio.h>
#include "DataTypes.h"

#define DIRECT 0
#define REG_DEST 1
#define REG_SRC 2
#define REG_ONLY 3
#define LABEL 4

int preRun(FILE* src, int *IC,int *DC,Label **labelTable,Data **dataMem,Inst **instrMem);
int get_opcode(char *str);
int isLabel(char *word,Label **LabelTable,int externFlag);
int isStringIns(char *s);
int isDataIns(char *s);
int letterCount(char *s,char letter);
void analyzeInstruction(int op, char *s,Inst **instrMem,int *IC,int line);
void updateLabels(Label **labelTable,int *IC);
void fillWord(Inst **instrMem,int value,int option,int wordNum);
