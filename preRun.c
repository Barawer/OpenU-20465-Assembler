
#include "preRun.h"
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"

int preRun(FILE* src,int *IC,int *DC,Label **labelTable,Data **dataMem,Inst **instrMem){
    char buff[MAX_LINE],word[MAX_LINE],temp[MAX_LINE],*strPtr,*numPtr;
    int line=1,stringLen,i,num,error,returnError=0;
    int labelFlag,stringFlag,dataFlag,op,externFlag,lineComplete,commandError;
    while(fgets(buff,MAX_LINE,src)!=NULL){
        strcpy(temp,buff);
        /*zero all flags*/
        labelFlag=0;
        lineComplete=0;
        stringFlag=0;
        dataFlag=0;
        externFlag=0;
        op=-1;
        commandError=0;
        if(strlen(buff)>=MAX_LINE-1){
            errorPrinter(line,LONG_LINE);
            error=LONG_LINE;
            continue;
        }
        else if(error==LONG_LINE){
            error=0;
        }
        else if(buff[0]!=';'|| isEmpty(buff)) {
            error=0;
/*check what we have in the line*/
            while (get_word(temp, word) == 1) {
		if(error<0&&returnError==0){
			returnError=-1;
		}
                if (!strcmp(word, ".extern")) {
                    externFlag=1;
                    error=1;
                    lineComplete=1;
                }
                else if(!strcmp(word, ".entry")){
                    error=1;
                    lineComplete=1;
                }
                else{
                    if (labelFlag == 0 && (error = isLabel(word, labelTable, externFlag)) == 1) {
                        if (externFlag == 1) {
                            (*labelTable)[labelTable[0]->totLabel - 1].isExternal = 1;
                            externFlag = 0;
                        }
                        labelFlag = 1;
                        lineComplete=1;
                    } else if (error<0) {
                        errorPrinter(line, error);
                        break;
                    }
                }
                if (isStringIns(word) ==1) {/*if this or data is true, then its data. else its instructions. inst=(ic)>data=(dc)*/
                    stringFlag = 1;
                } else if (isDataIns(word) == 1) {
                    dataFlag = 1;
                }
                op = get_opcode(word);
                /*Reads the line, and checks what it should do*/
                if (stringFlag == 1&&!isStringIns(word)) {
/*this is a string, so read the string into memory*/
                    if (letterCount(word, '"') != 2) {
                        if(letterCount(word,'"')>2){
                            errorPrinter(line,TOO_MANY_PARENTHESIS);
                            error=TOO_MANY_PARENTHESIS;
                            break;
                        }
                        else{
                            errorPrinter(line,MISSING_PARENTHESIS);
                            error=MISSING_PARENTHESIS;
                            break;
                        }
                    }
                    else{
                        if(labelFlag==1){
                            (*labelTable)[labelTable[0]->totLabel - 1].isData=1;
                            (*labelTable)[labelTable[0]->totLabel - 1].location = (*DC);
                            labelFlag=0;
                        }
                        stringLen=strlen(word);
                        for(i=0;i<=stringLen;i++){
                            if(word[i]!='"'){
                                (*dataMem)=realloc(*dataMem,sizeof(Data)*(1+(*dataMem)->size));
                                (*dataMem)[(*dataMem)->size].data=word[i];
                                (*dataMem)->size++;
                                (*DC)++;
                            }
                        }
                    }
                    stringFlag=0;
                    lineComplete=1;
                    }
                    else if(dataFlag==1){
/*this is a data line, so read the data into memory*/
                        if(labelFlag==1){
                            (*labelTable)[labelTable[0]->totLabel - 1].isData=1;
                            (*labelTable)[labelTable[0]->totLabel - 1].location = (*DC);
                            labelFlag=0;
                        }
                        numPtr=strtok(temp,",");
                        while(numPtr!=NULL){
                            if((num=strtol(numPtr,&strPtr,10))) {
                                (*dataMem) = realloc(*dataMem, sizeof(Data) * (1 + (*dataMem)->size));
                                (*dataMem)[(*dataMem)->size].data = num;
                                (*dataMem)->size++;
                                (*DC)++;
                            }
                            numPtr = strtok(NULL, ",");
                        }
                        dataFlag=0;
                        lineComplete=1;
                    }
                    else if(op!=-1){
/*this is an instruction, so analyze it*/
                        if(labelFlag==1){
                            (*labelTable)[labelTable[0]->totLabel - 1].isIns=1;
                            (*labelTable)[labelTable[0]->totLabel - 1].location = (*IC);
                            labelFlag=0;
                        }
                        analyzeInstruction(op,temp,instrMem,IC,line);
                    lineComplete=1;
                    }
                    if(labelFlag!=1 && dataFlag!=1 && stringFlag!=1 && lineComplete!=1&&commandError==0){
                        commandError=1;
                        errorPrinter(line,UNKNOWN_COMMAND);
                        error=UNKNOWN_COMMAND;
                    }
            }
        }
        line++;
    }
/*update label locations*/
    updateLabels(labelTable,IC);
return returnError;
}


int isLabel(char *s,Label **labelTable,int externFlag){
    int len,i;
len=strlen(s);
    if(externFlag!=1){
        if(len>30){
            /*if it is, check its length*/
            return LONG_LABEL;
        }
        /*check if we got a solo :, which means there was a space*/
        if(s[0]==':'){
                return SPACE_IN_LABEL;
        }
        /*check if it's a label*/
        if(s[len-1]!=':'){
            return 0;
        }
        else{
            if(!isalpha(s[0])){
                return WRONG_LABEL_FORMAT;
            }
        }
        s[strlen(s)-1]='\0';
    }
    /*check if the label already exist*/
    for(i=0;i<labelTable[0]->totLabel;i++){
        if(!strcmp(s,(*labelTable)[i].name)){
            /*label already exists*/
            return USED_LABEL;
        }
    }
	/*update label table*/
    *labelTable=realloc(*labelTable,sizeof(Label)*(i+1));
    (*labelTable)[(*labelTable)->totLabel].name=calloc(1,(sizeof (char*))*(len+1));
    strcpy((*labelTable)[(*labelTable)->totLabel].name,s);
    (*labelTable)->totLabel++;
    return 1;
}

void updateLabels(Label **labelTable,int *IC){
    int i;
    for(i=0;i<labelTable[0]->totLabel;i++){
        if((*labelTable)[i].isData){
            (*labelTable)[i].location+=(*IC);
        }
    }
}

void analyzeInstruction(int op,char *s,Inst **instrMem,int *IC,int line) {
    int i = 1, num,incr=0;
    char *token, operand[32], *strPtr, *numPtr={0},label[30];

    if (op==0 || op==1 || op==2 || op==3 || op==6) {
        if ((*instrMem)->size != 0) {
            (*instrMem)->size++;
        }
        *instrMem=allInst(instrMem,3);
        memset(&(*instrMem)[(*instrMem)->size].data, 0, sizeof(Word));
        token = strtok(s, ",");
        (*instrMem)[(*instrMem)->size].data.opcode = op;
        (*IC)++;
        while (token != NULL) {
	/*check parameters*/	
            while (get_word(token, operand) == 1) {
                memset(&(*instrMem)[(*instrMem)->size + i].data, 0, sizeof(Word));
                if (i == 1) {
		/*direct method*/
                    if (operand[0] == '#') {
                        if(op==6){
                            errorPrinter(line,INVALID_ADDRESSING);
                        }
                        else {
                            (*instrMem)[(*instrMem)->size].data.src = 0;
                            if ((num = strtol(operand + 1, &strPtr, 10))) {
                                fillWord(instrMem, num, DIRECT, i);
                                (*IC)++;
                                incr++;
                            }
                        }
		/*register method*/
                    } else if (operand[0] == 'r') {
                        if (isdigit(operand[1]) && operand[1] < '8') {
                            if(op==6){
                                errorPrinter(line,INVALID_ADDRESSING);
                            }
                            else{
                                (*instrMem)[(*instrMem)->size].data.src = 3;
                                (*IC)++;
                                incr++;
                                fillWord(instrMem, operand[1] - '0', REG_SRC, i);
                            }
                        }
		/*label method*/
                    } else if (operand[0] != '\0'&&operand[0]!=',') {
                        /*leaves this as a sign that the label needs to be checked*/
                        (*instrMem)[(*instrMem)->size].data.src = 1;
                        (*IC)++;
                        incr++;
                        (*instrMem)[(*instrMem)->size + 1].data.ERA = 3;
                    }
		/*bad parameter*/
                    else{
                        errorPrinter(line,INVALID_PARAMS);
                        break;
                    }
                }
                    /*changes everything to be reflected onto dest*/
                else if (i == 2) {
                    if (operand[0] == '#') {
                        if(op!=1){
                            errorPrinter(line,INVALID_ADDRESSING);
                        }
                        (*instrMem)[(*instrMem)->size].data.dest = 0;
                        if ((num = strtol(numPtr, &strPtr, 10))) {
                            fillWord(instrMem, num, DIRECT, i);
                            (*IC)++;
                            incr++;
                        }
                    } else if (operand[0] == 'r') {
                        (*instrMem)[(*instrMem)->size].data.dest = 3;
                        if (isdigit(operand[1]) && operand[1] < '8' && (*instrMem)[(*instrMem)->size].data.src == 3) {
                            fillWord(instrMem, operand[1] - '0', REG_ONLY, i - 1);
                        } else if (isdigit(operand[1]) && operand[1] < '8') {
                            fillWord(instrMem, operand[1] - '0', REG_DEST, i);
                            (*IC)++;
                            incr++;
                        }
                    } else if (operand[0] != '\0') {
                        (*instrMem)[(*instrMem)->size].data.dest = 1;
                        /*leaves this as a sign that the label needs to be checked*/
                        (*instrMem)[(*instrMem)->size + 2].data.ERA = 3;
                        (*IC)++;
                        incr++;
                    }
                }
            }
            i++;
            token = strtok(NULL, ",");
        }
        (*instrMem)->size=(*instrMem)->size+incr;
        if (i > 3) {
            errorPrinter(line,TOO_MANY_PARAMETERS);
        } else if (i < 3) {
            errorPrinter(line,NOT_ENOUGH_PARAMETERS);
        }

    }
    if (op==4 || op==5 || (op>=7 && op<14)) {
        if ((*instrMem)->size != 0) {
            (*instrMem)->size++;
        }
        *instrMem=allInst(instrMem,1);
        memset(&(*instrMem)[(*instrMem)->size].data, 0, sizeof(Word));
        (*instrMem)[(*instrMem)->size].data.opcode = op;
        (*IC)++;
        if(op==9 || op==10 || op==13){
            if(strchr(s,'(')!=NULL){
                *instrMem=allInst(instrMem,3);
                /*separate the label from the parameters*/
                if(get_word(s, operand)){
                    for(i=0;i< strlen(operand);i++){
                        if(operand[i]=='('){
                            label[i]='\0';
                            operand[i]=' ';
                            i=strlen(operand)+1;
                        }
                        else{
                            label[i]=operand[i];
                        }
                    }
                    memmove(operand,operand+strlen(label)+1,strlen(operand)-strlen(label)-1);
                    operand[strlen(operand)-strlen(label)-1]='\0';
                    /*get the parameters*/
                    i=1;
                    (*instrMem)[(*instrMem)->size].data.dest = 2;
                    (*instrMem)[(*instrMem)->size + 1].data.ERA = 3;
                    (*IC)++;
                    (*instrMem)->size++;
                    token = strtok(operand, ",");
                    while (token != NULL) {
                            memset(&(*instrMem)[(*instrMem)->size + i].data, 0, sizeof(Word));
                            if (i == 1) {
                                if (token[0] == '#') {
                                    (*instrMem)[(*instrMem)->size-1].data.par1 = 0;
                                    if ((num = strtol(token + 1, &strPtr, 10))) {
                                        fillWord(instrMem, num, DIRECT, i);
                                        (*IC)++;
                                        incr++;
                                    }
                                } else if (token[0] == 'r') {
                                    if (isdigit(token[1]) && token[1] < '8') {
                                        (*instrMem)[(*instrMem)->size-1].data.par1 = 3;
                                        fillWord(instrMem, token[1] - '0', REG_SRC, i);
                                        (*IC)++;
                                        incr++;
                                    }
                                } else if (token[0] != '\0') {
                                    /*leaves this as a sign that the label needs to be checked*/
                                    (*instrMem)[(*instrMem)->size-1].data.par1 = 1;
                                    (*instrMem)[(*instrMem)->size + 1].data.ERA = 3;
                                    (*IC)++;
                                    incr++;
                                }
                                else if(token[0]==','){
                                    errorPrinter(line,INVALID_PARAMS);
                                }
                            }
                                /*changes everything to be reflected onto dest*/
                            else if (i == 2) {
                                if (token[0] == '#') {
                                    (*instrMem)[(*instrMem)->size-1].data.par2 = 0;
                                    if ((num = strtol(token + 1, &strPtr, 10))) {
                                        fillWord(instrMem, num, DIRECT, i);
                                        (*IC)++;
                                        incr++;
                                    }
                                } else if (token[0] == 'r') {
                                    (*instrMem)[(*instrMem)->size-1].data.par2 = 3;
                                    if (isdigit(token[1]) && token[1] < '8' && (*instrMem)[(*instrMem)->size-1].data.par1== 3) {
                                        fillWord(instrMem, token[1] - '0', REG_ONLY, i - 1);
                                    }
                                    else if (isdigit(token[1]) && token[1] < '8') {
                                        fillWord(instrMem, token[1] - '0', REG_DEST, i);
                                        (*IC)++;
                                        incr++;
                                    }
                                } else if (token[0] != '\0') {
                                    (*instrMem)[(*instrMem)->size-1].data.par2 = 1;
                                    /*leaves this as a sign that the label needs to be checked*/
                                    (*instrMem)[(*instrMem)->size + 2].data.ERA = 3;
                                    (*IC)++;
                                    incr++;
                                }
                            }
                        i++;
                        token = strtok(NULL, ",");
                    }
                    if (i > 3) {
                        errorPrinter(line,TOO_MANY_PARAMETERS);
                    } else if (i < 3) {
                        errorPrinter(line,NOT_ENOUGH_PARAMETERS);
                    }
                }
                (*instrMem)->size=(*instrMem)->size+incr;
            }
        }
        if(strchr(s,',')!=NULL){
            errorPrinter(line,TOO_MANY_PARAMETERS);
        }
        if (get_word(s, operand)==1) {
                if (operand[0] == '#') {
                    if(op!=12){
                        errorPrinter(line,INVALID_ADDRESSING);
                    }
                    else{
                        (*instrMem)[(*instrMem)->size].data.dest = 0;
                        if ((num = strtol(operand + 1, &strPtr, 10))) {
                            fillWord(instrMem, num, DIRECT, 1);
                            (*IC)++;
                        }
                    }
                } else if (operand[0] == 'r') {
                    (*instrMem)[(*instrMem)->size].data.dest = 3;
                    if (isdigit(operand[1]) && operand[1] < '8') {
                        fillWord(instrMem, operand[1] - '0', REG_DEST, 1);
                        (*IC)++;
                    }
                } else if (operand[0] != '\0') {
                    (*instrMem)[(*instrMem)->size].data.dest = 1;
                    memset(&(*instrMem)[(*instrMem)->size+1].data, 0, sizeof(Word));
                    /*leaves this as a sign that the label needs to be checked*/
                    (*instrMem)[(*instrMem)->size + 1].data.ERA = 3;
                    (*IC)++;
                }
                (*instrMem)->size++;
            }
        }
    if (op >= 14 && op <= 15) {
        if(get_word(s,operand)==1){
            errorPrinter(line,TOO_MANY_PARAMETERS);
        }
        else{
            *instrMem=allInst(instrMem,1);
            (*instrMem)->size++;
            (*instrMem)[(*instrMem)->size].data.opcode = op;
            (*IC)++;
            }
        }
}

int isStringIns(char *s){
    return !strcmp(".string",s);
}
int isDataIns(char *s){
    return !strcmp(".data",s);
}

int get_opcode(char *str){
    if(!strcmp(str,"mov")){
        return 0;
    }
    if(!strcmp(str,"cmp")){
        return 1;
    }
    if(!strcmp(str,"add")){
        return 2;
    }
    if(!strcmp(str,"sub")){
        return 3;
    }
    if(!strcmp(str,"not")){
        return 4;
    }
    if(!strcmp(str,"clr")){
        return 5;
    }
    if(!strcmp(str,"lea")){
        return 6;
    }
    if(!strcmp(str,"inc")){
        return 7;
    }
    if(!strcmp(str,"dec")){
        return 8;
    }
    if(!strcmp(str,"jmp")){
        return 9;
    }
    if(!strcmp(str,"bne")){
        return 10;
    }
    if(!strcmp(str,"red")){
        return 11;
    }
    if(!strcmp(str,"prn")){
        return 12;
    }
    if(!strcmp(str,"jsr")){
        return 13;
    }
    if(!strcmp(str,"rts")){
        return 14;
    }
    if(!strcmp(str,"stop")){
        return 15;
    }
    return -1;
}
/*counts the letters, used to check is the string is legal*/
int letterCount(char *s,char letter){
    int len=strlen(s);
    int i,counter=0;
    for (i=0;i<len;i++){
        if(s[i]==letter){
            counter++;
        }
    }
    return counter;
}

void fillWord(Inst **instrMem,int value,int option,int wordNum){
    if(value<0){
        value=convertTo2(value);
    }
    switch(option){
        case DIRECT:
            (*instrMem)[(*instrMem)->size+wordNum].data.ERA=0;
            (*instrMem)[(*instrMem)->size+wordNum].data.dest=(value);
            (*instrMem)[(*instrMem)->size+wordNum].data.src=(value>>2);
            (*instrMem)[(*instrMem)->size+wordNum].data.opcode=(value>>4);
            (*instrMem)[(*instrMem)->size+wordNum].data.par2=(value>>8);
            (*instrMem)[(*instrMem)->size+wordNum].data.par1=(value>>10);
            break;
        case REG_DEST:
            (*instrMem)[(*instrMem)->size+wordNum].data.dest=value;
            (*instrMem)[(*instrMem)->size+wordNum].data.src=(value>>2);
            (*instrMem)[(*instrMem)->size+wordNum].data.opcode=(value>>4);
            break;
        case REG_SRC:
            (*instrMem)[(*instrMem)->size+wordNum].data.opcode=12&(value<<2);
            (*instrMem)[(*instrMem)->size+wordNum].data.par2=(value>>2);
            (*instrMem)[(*instrMem)->size+wordNum].data.par1=(value>>4);
            break;
        case REG_ONLY:
            (*instrMem)[(*instrMem)->size+wordNum].data.dest=value;
            (*instrMem)[(*instrMem)->size+wordNum].data.src=(value>>2);
            (*instrMem)[(*instrMem)->size+wordNum].data.opcode=(value>>4)|(*instrMem)[(*instrMem)->size+wordNum].data.opcode;
            break;
        case LABEL:
            (*instrMem)[wordNum].data.dest=(value);
            (*instrMem)[wordNum].data.src=(value>>2);
            (*instrMem)[wordNum].data.opcode=(value>>4);
            (*instrMem)[wordNum].data.par2=(value>>8);
            (*instrMem)[wordNum].data.par1=(value>>10);
            break;
    }
}
