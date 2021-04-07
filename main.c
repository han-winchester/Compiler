// Created by Han Winchester
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

int main(int argc, char *argv[]){

	int lflag = 0; int aflag = 0; int vflag = 0; int i =0;

	// user doesn't include file name in command line
	if(argc < 2){
		printf("Error: please include the file name.\n");
		return 0;
	}
	// user inputs all directives in command line
	else if(argc > 4){
		lflag = 1;
		aflag = 1;
		vflag = 1;
	}
	//checks if arguments have 3 or 4 values inputted
	else if(argc == 3 || argc == 4){
		if(!strcmp(argv[2], "-l")){ lflag = 1; }
		else if(!strcmp(argv[2], "-a")){ aflag = 1; }
		else{ vflag = 1; }

		if(argc == 4){
			if(!strcmp(argv[3], "-l")){ lflag = 1; }
			else if(!strcmp(argv[3], "-a")){ aflag = 1; }
			else{ vflag = 1; }
		}
	}

	FILE * inputFile = fopen(argv[1], "r");
	// cant read input file
	if(inputFile == NULL){ // error
		printf("Error: Couldn't open input file\n");
		printf("Exited Program.\n");
		exit(0); // quits program
	}
	// doesnt print to screen if no commands were detected
	//if(lflag == 0 && aflag == 0 && vflag == 0){ exit(0); }

	// passes in the input file and flags to lex, this then calls parser and vm with these flags as well
    lex(inputFile, lflag, aflag, vflag);
    
}