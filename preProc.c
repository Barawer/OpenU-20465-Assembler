#include "preProc.h"
#include "utils.h"
#include <stdio.h>

/*
todo - check saved word list*/

int runPreProcess(FILE* src, FILE* dest){
	int i,j,lineLen=0,readingMac=0,writingMac=0,endReading=0;
	char buff[MAX_LINE],word[MAX_LINE],temp[MAX_LINE];
	char startMac[]="mcr",endMac[]="endmcr";
    Macro *mcrLst=malloc(sizeof(Macro));
    int totMacros=0,currLines=0;

	while(fgets(buff,MAX_LINE,src)!=NULL){
        lineLen=strlen(buff);
        strcpy(temp,buff);
        if(endReading==1){
            endReading=0;
        }
        if(readingMac==0){
            if(get_word(temp,word)==1 && !strcmp(word, startMac) ) {
                readingMac = 1;
                currLines=0;
                if (totMacros != 0) {
                    mcrLst = realloc(mcrLst, sizeof(Macro) * (totMacros + 1));
                }
                if(get_word(temp,word)==1){
                    mcrLst[totMacros].name=malloc(strlen(word));
                    strcpy(mcrLst[totMacros].name,word);
                }

            }
        }
        else {
            if (get_word(temp,word)==1 && strcmp(word, endMac)) {
                if(currLines==0){
                    mcrLst[totMacros].lines= malloc(sizeof(char*)*(currLines+1));
                }
                else{
                    mcrLst[totMacros].lines= realloc(mcrLst[totMacros].lines,sizeof(char*)*(currLines+1));
                }
                mcrLst[totMacros].lines[currLines]=malloc(lineLen+1);
                memcpy(mcrLst[totMacros].lines[currLines], buff,lineLen+1);
                mcrLst[totMacros].totLines++;
                currLines++;
            }
            else if(!strcmp(word, endMac)){
                readingMac=0;
                endReading=1;
                totMacros++;
            }
        }
        writingMac=0;
        strcpy(temp,buff);
        if(get_word(temp,word)==1&& strcmp(word,endMac) && strcmp(word,startMac)){
            for(i=0;i<totMacros;i++){
                if(!strcmp(word,mcrLst[i].name)){
                    writingMac=1;
                    for(j=0;j<mcrLst[i].totLines;j++){
                        fprintf(dest,"%s",mcrLst[i].lines[j]);
                    }

                }
            }
        }
        if(writingMac==0&&readingMac!=1&&endReading!=1){
            fprintf(dest,"%s",buff);
        }

	}
    for(i=0;i<totMacros;i++){
        free(mcrLst[i].lines);
        free(mcrLst[i].name);
    }
    free(mcrLst);
    return 0;
}

