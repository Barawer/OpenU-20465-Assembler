#include "utils.h"

int finalRun(FILE* src,char *filename,Label **labelTable,Inst **instrMem);
int checkSkips(char *s);
int findLabel(char *s,Label **labelTable,int line);
int analyzeInstructionFinal(int op,char *s,Inst **instrMem,FILE *ext,Label **labelTable,int line);
int findFirstLabel(Inst **instrMem);
int createEn(char *filename,Label **labelList);
