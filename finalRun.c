#include "finalRun.h"
#include "preRun.h"
#include <ctype.h>

int finalRun(FILE* src,char *filename,Label **labelTable,Inst **instrMem){
    char buff[MAX_LINE],word[MAX_LINE],temp[MAX_LINE];
    int labelNum,line=1,error=0,returnError=0;
    int op;
    char ex[80];
    FILE *ext;
    strcpy(ex,filename);
    ex[strlen(filename)-3]='\0';
    strcat(ex,".ext");
    if((ext=fopen(ex,"w+"))==NULL){
        fprintf(stderr,"Error! could not create externals file\n");
	returnError=-1;
        return returnError;
    }
    while(fgets(buff,MAX_LINE,src)!=NULL) {
        strcpy(temp, buff);
        if (buff[0] != ';' || !isEmpty(buff)) {
            while (get_word(temp, word) == 1 && checkSkips(word)) {
		/*mark entry labels*/
                if(!strcmp(word,".entry")){
                    if(get_word(temp,word)==1){
                        if((labelNum=findLabel(word,labelTable,line))!=-1){
                            (*labelTable)[labelNum].isEnt=1;
                        }
                    }
                }
		/*checks if the first word is a label, if it is, continue to check if its a command*/
                else if(findLabel(word,labelTable,line)!=-1){
                    if(get_word(temp, word) == 1) {
                        op = get_opcode(word);
                        error=analyzeInstructionFinal(op, temp, instrMem,ext, labelTable,line);
                    }
                }
                else{
                    op = get_opcode(word);
                    error=analyzeInstructionFinal(op, temp, instrMem,ext, labelTable,line);
                }
		if(error!=0){
			returnError=-1;
		}
            }
        }
        line++;
    }
	if(returnError==0){
    		createEn(filename,labelTable);
 		fclose(ext);
	}
	else{
    		fclose(ext);
		remove(ex);
	}
	return returnError;
}

/*checks if those are known words that we dont need to check, in order to skip*/
int checkSkips(char *s) {
    if(!strcmp(".extern",s)){
        return 0;
    }
    else if(!strcmp(".data",s)){
        return 0;
    }
    else if(!strcmp(".string",s)){
        return 0;
    }
    return 1;
}

/*find label in label table and return its location*/
int findLabel(char *s,Label **labelTable,int line){
    int i;
    if(s[strlen(s)-1]==':'){
        s[strlen(s)-1]='\0';
    }
    if(isspace(s[strlen(s)-1])){
        s[strlen(s)-1]='\0';
    }
    for(i=0;i<labelTable[0]->totLabel;i++){
        if(!strcmp(s,(*labelTable)[i].name)){
            return i;
        }
    }
    return -1;
}

/*find first empty label slot in memory and return its location*/
int findFirstLabel(Inst **instrMem){
    int i;
    for(i=0;i<(*instrMem)->size;i++){
        if((*instrMem)[i].data.ERA==3&&(*instrMem)[i].data.src==0&&(*instrMem)[i].data.dest==0\
        &&(*instrMem)[i].data.opcode==0&&(*instrMem)[i].data.par1==0&&(*instrMem)[i].data.par2==0){
            return i;
        }
    }
    return -1;
}

int analyzeInstructionFinal(int op,char *s,Inst **instrMem,FILE *ext,Label **labelTable,int line) {
    int i,j=0, num, incr = 0,memLoc;
    char *token, operand[32],label[30];
    if (op==0 || op==1 || op==2 || op==3 || op==6) {
        incr++;
        token = strtok(s, ",");
        while (token != NULL) {
            while (get_word(token, operand) == 1) {
                if (operand[0] != '#' && operand[0] != 'r') {
                    if ((num = findLabel(operand, labelTable,line)) != -1) {
                        if ((memLoc=findFirstLabel(instrMem))!=-1&& (*instrMem)[memLoc].data.ERA == 3) {
			/*update memory in the correct location, based on label location*/
                            if((*labelTable)[num].isExternal==1){
                                fprintf(ext,"%s\t%d\n",(*labelTable)[num].name,num);
                                (*instrMem)[memLoc].data.ERA=1;
                            }
                            else{
                                (*instrMem)[memLoc].data.ERA=2;
                                fillWord(instrMem, 100+(*labelTable)[num].location, LABEL, memLoc);
                            }
                        }
                        incr++;
                    }
                    else{
                        errorPrinter(line,LABEL_DOESNT_EXIST);
                        return -1;
                    }
                } else {
                    incr++;
                }
            }
            token = strtok(NULL, ",");
        }
    }
    if (op==4 || op==5 || (op>=7 && op<14)) {
        if (op == 9 || op == 10 || op == 13) {
            if (strchr(s, '(') != NULL) {
                /*separate the label from the parameters*/
                if (get_word(s, operand)) {
                    for (i = 0; i < strlen(operand); i++) {
                        if (operand[i] == '(') {
                            label[i] = '\0';
                            operand[i] = ' ';
                            j=i;
                        } else if (j==0) {
                            label[i] = operand[i];
                        }
                        else if (operand[i]==')'){
                            operand[i] = ' ';
                        }
                    }
                    memmove(operand, operand + strlen(label) + 1, strlen(operand) - strlen(label) - 1);
                    operand[strlen(operand) - strlen(label) - 1] = '\0';
                    if ((num = findLabel(label, labelTable,line)) != -1) {
                        if ((memLoc = findFirstLabel(instrMem)) != -1 && (*instrMem)[memLoc].data.ERA == 3) {
                            if ((*labelTable)[num].isExternal == 1) {
                                fprintf(ext,"%s\t%d\n",(*labelTable)[num].name,100+memLoc);
                                (*instrMem)[memLoc].data.ERA = 1;
                            } else {
                                (*instrMem)[memLoc].data.ERA = 2;
                                fillWord(instrMem, 100 + (*labelTable)[num].location, LABEL, memLoc);
                            }
                        }
                    }
                    else {
                        errorPrinter(line, LABEL_DOESNT_EXIST);
                        return -1;
                    }
                    /*get the parameters*/
                        token = strtok(operand, ",");
                        while (token != NULL) {
                            if (token[0] != '#' && token[0] != 'r') {
                                if ((num = findLabel(token, labelTable,line)) != -1) {
                                    if ((memLoc=findFirstLabel(instrMem))!=-1&& (*instrMem)[memLoc].data.ERA == 3) {
                                        if((*labelTable)[num].isExternal==1){
                                            fprintf(ext,"%s\t%d\n",(*labelTable)[num].name,100+memLoc);
                                            (*instrMem)[memLoc].data.ERA=1;
                                        }
                                        else{
                                            (*instrMem)[memLoc].data.ERA=2;
                                            fillWord(instrMem, 100+(*labelTable)[num].location, LABEL, memLoc);
                                        }
                                    }
                                }
                            }
                            token = strtok(NULL, ",");
                        }
                }
            }
        }
        if (get_word(s, operand) == 1) {
            if (operand[0] != '#' && operand[0] != 'r') {
                if ((num = findLabel(operand, labelTable,line)) != -1) {
                    if ((memLoc=findFirstLabel(instrMem))!=-1&& (*instrMem)[memLoc].data.ERA == 3) {
                        if((*labelTable)[num].isExternal==1){
                            fprintf(ext,"%s\t%d\n",(*labelTable)[num].name,100+memLoc);
                            (*instrMem)[memLoc].data.ERA=1;
                        }
                        else{
                            (*instrMem)[memLoc].data.ERA=2;
                            fillWord(instrMem, 100+(*labelTable)[num].location, LABEL, memLoc);
                        }
                    }
                }
            }
        }
    }
	return 0;
}

int createEn(char *filename,Label **labelList){
    int nameLen=strlen(filename),i;
    char en[80];
    FILE *ent;
    /*since we are only receiving a .am file, we know we can ignore the last 3 chars*/
    strcpy(en,filename);
    en[nameLen-3]='\0';
    strcat(en,".ent");
    if((ent=fopen(en,"w+"))==NULL){
        fprintf(stderr,"Error! could not create entries source file\n");
        return -1;
    }
    for(i=0;i<(*labelList)->totLabel;i++){
        if((*labelList)[i].isEnt==1){
            fprintf(ent,"%s\t%d\n",(*labelList)[i].name,100+(*labelList)[i].location);
        }
    }
    fclose(ent);
    return 1;
}
