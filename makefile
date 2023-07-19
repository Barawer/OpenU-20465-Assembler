assembler: assembler.o preProc.o utils.o preRun.o finalRun.o
	gcc -ansi -Wall -g assembler.o preProc.o preRun.o utils.o finalRun.o -o assembler
assembler.o: assembler.c preProc.h
	gcc -c -ansi -Wall -g assembler.c
preProc.o: preProc.c preProc.h utils.h
	gcc -c -ansi -Wall -g preProc.c
utils.o: utils.h
	gcc -c -ansi -Wall -g utils.c
preRun.o: preRun.c preRun.h utils.h
	gcc -c -ansi -Wall -g preRun.c
finalRun.o: finalRun.c finalRun.h preRun.h utils.h
	gcc -c -ansi -Wall -g finalRun.c