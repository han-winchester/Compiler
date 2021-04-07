// Created by Han Winchester
// header file 

typedef struct instructions{
	char op[12]; 
	int level;   
	int m;       
	int opcode;  
	int opNum;
}instructions;

void lex(FILE * inputFile, int lflag, int aflag, int vflag);

void parser(char tokens[], int aflag, int vflag);

void vm(struct instructions codeText[], int vflag);