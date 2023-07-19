#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "preProc.h"
#include "preRun.h"
#include "utils.h"
#include "finalRun.h"

char savedWordList[]={"r0,r1,r2,r3,r4,r5,r6,r7"};

int main(int argc, char *argv[]){
	FILE *src, *dest;
    int IC=0,DC=0;
	char filename[80];
	int currfile=0;
    Label *labelTable=calloc(1,sizeof(Label));
    Data *dataMem=calloc(1,sizeof(Data));
    Inst *instrMem=calloc(1,sizeof(Inst));
/*check that we got a file*/
    if(argc<2){
		fprintf(stderr,"No input file provided\n");
	}
	while(++currfile<argc){
        /*read file + create file*/
        strcpy(filename,argv[currfile]);
        strcat(filename,".as");
        if((src=fopen(filename,"r"))==NULL){
			fprintf(stderr,"Error! could not open source file %s\n",argv[currfile]);
			break;
		}
        filename[strlen(filename)-1]='m';
		if((dest=fopen(filename,"w+"))==NULL){
			fprintf(stderr,"Error! could not create expanded source file%s\n",argv[currfile]);
			break;
		}
        runPreProcess(src,dest);
        fclose(src);
        fclose(dest);

        /*run first pass*/
        if((dest=fopen(filename,"r"))==NULL){
            fprintf(stderr,"Error! could not open expanded source file%s\n",filename);
            break;
        }
        if(preRun(dest,&IC,&DC,&labelTable,&dataMem,&instrMem)!=0){
		freeAll(&instrMem,&dataMem,&labelTable);
		fclose(dest);
		break;
	}
        fseek(dest,0,SEEK_SET);
        /*run second pass*/
        if(finalRun(dest,filename,&labelTable,&instrMem)!=0){
		freeAll(&instrMem,&dataMem,&labelTable);
		fclose(dest);
		break;
	}
        fclose(dest);
        
        /*print file + free Memory*/
        printMem(&instrMem,&dataMem,IC, DC,filename);
        freeAll(&instrMem,&dataMem,&labelTable);
    }
	return 0;	
}
