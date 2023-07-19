#ifndef STRC
#define STRC
typedef struct Data{
    int data;
    int size;
}Data;

typedef struct Word{
    unsigned int par1:2;
    unsigned int par2:2;
    unsigned int opcode:4;
    unsigned int src:2;
    unsigned int dest:2;
    unsigned int ERA:2;
}Word;

typedef struct Inst{
    Word data;
    int size;
}Inst;


typedef struct Label{
    char* name;
    int location;
    int isExternal;
    int isData;
    int isIns;
    int isEnt;
    int totLabel;
}Label;

#endif
