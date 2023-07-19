
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

int get_word(char *word,char *command){
    int i=0,j;
    int w_start;
    int w_end=-1;
    int word_len=strlen(word);
    int lim=MAX_LINE;
    while(isspace(word[i])){
        i++;
    }
    w_start=i;
    if(word[i]=='-'&&word[i+1]=='1'){
        i=i+2;
    }
    for(;--lim>0;i++){
        if(!isalpha(word[i])&& !ispunct(word[i])&&!isdigit(word[i])){
            i--;
            w_end=i;
            break;
        }
    }
    if(w_end==-1||word[i]=='\0'||word[i]=='\n'){
        return -1;
    }
    for(j=0;j<w_end+1-w_start;j++){
        command[j]=word[w_start+j];
    }
    if(word_len-1-(w_end+1-w_start)-w_start<=0){
        word[0]='\0';
    }
    else{
        memmove(word,word+(w_end-w_start)+2+w_start,word_len-(w_end-w_start)+1);
        word[word_len-(w_end-w_start)]='\0';
    }
    command[j]='\0';
    return 1;
}

int isEmpty(char* s){
    while(*s){
        if(isspace(*s)){
            return 0;
        }
        s++;
    }
    return 1;
}

void printMem(Inst **instMem,Data **DataMem,int IC, int DC,char *filename){
    FILE *fp;
    int i,j,x,line=100;
    filename[strlen(filename)-3]='\0';
    strcat(filename,".ob");
    fp=fopen(filename,"w+");
    fprintf(fp,"\t%d\t%d\n",IC,DC);
    for(i=0;i<IC;i++){
        fprintf(fp,"%d\t\t",line);
        line++;
        for(j=1;j>=0;j--){
            x=(*instMem)[i].data.par1;
            fputc(x & (1u << j) ? '/' : '.',fp);
        }

        for(j=1;j>=0;j--){
            x=(*instMem)[i].data.par2;
            fputc(x & (1u << j) ? '/' : '.',fp);
        }

        for(j=3;j>=0;j--){
            x = (*instMem)[i].data.opcode;
            fputc(x & (1u << j) ? '/' : '.',fp);
        }

        for(j=1;j>=0;j--){
            x=(*instMem)[i].data.src;
            fputc(x & (1u << j) ? '/' : '.',fp);
        }

        for(j=1;j>=0;j--){
            x=(*instMem)[i].data.dest;
            fputc(x & (1u << j) ? '/' : '.',fp);
        }

        for(j=1;j>=0;j--){
            x=(*instMem)[i].data.ERA;
            fputc(x & (1u << j) ? '/' : '.',fp);
        }
        fputc('\n',fp);
    }
    for(i=0;i<DC;i++){
        fprintf(fp,"%d\t\t",line);
        line++;
        for(j=13;j>=0;j--){
            x=(*DataMem)[i].data;
            fputc(x & (1u << j) ? '/' : '.',fp);
        }
        fputc('\n',fp);
    }
    fclose(fp);

}

int convertTo2(int x){
    int result;
    int negX=-x;
    int mask=(1<<14)-1;
    int last14bits =negX&mask;
    result=(0|((~last14bits)+1));
    return result;
}

void freeAll(Inst **instMem,Data **dataMem, Label **labelTable){
    int i;
    free((*instMem));
    free((*dataMem));
    for(i=0;i<(*labelTable)[0].totLabel;i++){
       free((*labelTable)[i].name);
    }
    free((*labelTable));
}

void errorPrinter(int line, int errorType){
    switch(errorType){
        case LONG_LABEL:
            fprintf(stdout,"Error in line %d: Label exceeds maximum size\n",line);
            break;
        case SPACE_IN_LABEL:
            fprintf(stdout,"Error in line %d: Label contains spaces\n",line);
            break;
        case USED_LABEL:
            fprintf(stdout,"Error in line %d: Label is already defined\n",line);
            break;
        case WRONG_LABEL_FORMAT:
            fprintf(stdout,"Error in line %d: Label doesnt not start with an alphanumeric character\n",line);
            break;
        case TOO_MANY_COMMAS:
            fprintf(stdout,"Error in line %d: More commas than parameters\n",line);
            break;
        case INVALID_PARAMS:
            fprintf(stdout,"Error in line %d: Line contains empty or invalid parameters\n",line);
            break;
        case LONG_LINE:
            fprintf(stdout,"Error in line %d: Line exceeds maximum size\n",line);
            break;
        case NO_DATA:
            fprintf(stdout,"Error in line %d: No data after '.data'\n",line);
            break;
        case STRING_IN_DATA:
            fprintf(stdout,"Error in line %d: Strings or chars in a '.data' line\n",line);
            break;
        case DOUBLE_SIGNS:
            fprintf(stdout,"Error in line %d: One or more Parameters contains a double sign\n",line);
            break;
        case SPACE_IN_PARAMS:
            fprintf(stdout,"Error in line %d: Parameters contains space\n",line);
            break;
        case NOT_AN_INT:
            fprintf(stdout,"Error in line %d: One or more Parameters is not an integer\n",line);
            break;
        case MISSING_PARENTHESIS:
            fprintf(stdout,"Error in line %d: Missing parenthesis\n",line);
            break;
        case TOO_MANY_PARENTHESIS:
            fprintf(stdout,"Error in line %d: Too many parenthesis\n",line);
            break;
        case NOT_ENOUGH_PARAMETERS:
            fprintf(stdout,"Error in line %d: Not enough parameters\n",line);
            break;
        case TOO_MANY_PARAMETERS:
            fprintf(stdout,"Error in line %d: Too many parameters\n",line);
            break;
        case UNKNOWN_COMMAND:
            fprintf(stdout,"Error in line %d: Unknown command\n",line);
            break;
        case INVALID_ADDRESSING:
            fprintf(stdout,"Error in line %d: This command does not support this addressing type\n",line);
            break;
        case LABEL_DOESNT_EXIST:
            fprintf(stdout,"Error in line %d: This label doesn't exist in the label table\n",line);
            break;
    }

}

Inst* allInst(Inst **instrMem,int newSize){
    if((*instrMem = realloc(*instrMem, sizeof(Inst) * (newSize+1+(*instrMem)->size)))!=NULL){
        return (*instrMem);
    }
    printf("Couldnt allocate Memory\n");
    return NULL;
}
