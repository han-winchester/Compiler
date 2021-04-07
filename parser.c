// Created by Han Winchester
// Takes in input from lex.c
// outputs code and send to vm.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

#define MAX_SYMBOL_TABLE_SIZE 500

typedef struct symbol{ 
	int kind; 		// const = 1, var = 2
	char name[12];	// name up to 11 chars
	int val; 		// number 
	int level; 		// L level
	int addr; 		// M address
	int mark;		// to indicate that code has been generated already for a block
	int param; 		// to indicate if the parameter for a procedure has been declared
} symbol; 

// Global
int procedureCount = 0;
int tokensId = 0; 
int symbol_table_id = 0;
int lineNum = 0; // also opcode, NOT LEXICAL GRAPHICAL LEVEL
int codeId = 0; 
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE] = {}; // initialize symbol table
instructions code[500] = {};

// function header declaration
int expression(char token, char tokens[]);

int outputAssembly(){
	int i = 0;

    printf("Generated Assembly: \n");
    printf("Line    OP   L   M\n");

	for(i=0;i<=lineNum;i++){ printf("%d	%s   %d   %d\n", code[i].opcode, code[i].op, code[i].level, code[i].m); } // outputs the assembly line by line

	printf("\n");
	
	return 1;
}

// insert into code array to be used in vm.c
int emit(int opcode, char op[], int level, int m){
	code[codeId].opcode = opcode; // line number
	strcpy(code[codeId].op, op);
	code[codeId].level = level;
	code[codeId].m = m;
	
	// find the op number from the respective op name; for vm
	if(!strcmp(code[codeId].op, "LIT")){
		code[codeId].opNum = 1;
	}
	else if(!strcmp(code[codeId].op, "OPR")){
		code[codeId].opNum = 2;
	}
	else if(!strcmp(code[codeId].op, "LOD")){
		code[codeId].opNum = 3;
	}
	else if(!strcmp(code[codeId].op, "STO")){
		code[codeId].opNum = 4;
	}
	else if(!strcmp(code[codeId].op, "CAL")){
		code[codeId].opNum = 5;
	}
	else if(!strcmp(code[codeId].op, "INC")){
		code[codeId].opNum = 6;
	}
	else if(!strcmp(code[codeId].op, "JMP")){
		code[codeId].opNum = 7;
	}
	else if(!strcmp(code[codeId].op, "JPC")){
		code[codeId].opNum= 8;
	}
	else{ // SYS
		code[codeId].opNum = 9;
	}

	codeId++;
	return 1;
}

// parameters: empty array for identifier name or number, list of tokens
// returns: the char pointer to identifier name or number
char* getIdentifier(char identName[], char tokens[]){
	int identNameId = 0;
	do{
		identName[identNameId] = tokens[tokensId]; // copy token from list to identName array
		identNameId++; tokensId++;
	}while(tokens[tokensId] > 47); // ascii value is greater than 47 (i.e. letter or number)
	return identName;
}

// parameters: list of tokens
// returns: the next token in the token list specified by tokensId
int getNextToken(char tokens[]){
	char nextToken = tokens[tokensId];
	tokensId++;
	return nextToken;
}

// linear search through symbol table looking at name
// return index if found -1 if not
int symbolTableCheck(char token, char tokens[]){
	char identifier[500] = {};
	int i = 0; int g = 0;

	do{
		token = getNextToken(tokens);

		// pass name into identifier to compare strings in symbol table
		identifier[g] = token;
		g++;
	}while(tokens[tokensId] > 47);

	int y = 0;
	// iterate through identifier array to decrement o to not skip over tokens in tokens[tokensId] when returning from this function
	while(identifier[y] != '\0'){
		y++;
		tokensId--;
	}

	// iterate through table array and check if name matches any entries
	for(i=0;i<=MAX_SYMBOL_TABLE_SIZE;i++){
		if(!(strcmp(identifier, symbol_table[i].name))){ // strcmp returns 0 if the strings match
			return i;
		}
	}
	return -1; // not found in symbol table
}

int constDeclaration(char token, char tokens[]){
	//constsym
	if(token == 28){
		do{
			char identName[500] = {};
			//int identNameId = 0;

			token = getNextToken(tokens);

			// check for identsym
			if(token != 2){
				printf("Error: const, var, procedure, call and read keywords must be followed by identifier\n");
				exit(0);
			}
			if(symbolTableCheck(token, tokens) != -1){
				printf("Error: symbol name has already been declared\n");
				exit(0);
			}

			// Get identifier name
			strcpy(identName, getIdentifier(identName, tokens));
	
			token = getNextToken(tokens);

			// check for equal symbol
			if(token != 9){
				printf("Error: constants must be assigned with =\n");
				exit(0);
			}

			token = getNextToken(tokens);

			// check for number
			if(token !=  3){
				printf("Error: constants must be assigned an integer value\n");
				exit(0);
			}

			char number[6] = {};
			// gets the number for the constant
			strcpy(number, getIdentifier(number, tokens));

			int numberDigit = atoi(number); // convert from char array to int

			token = getNextToken(tokens);

			// Add to symbol table
			symbol_table[symbol_table_id].kind = 1;
			strcpy(symbol_table[symbol_table_id].name, identName);
			symbol_table[symbol_table_id].val = numberDigit; 
			symbol_table[symbol_table_id].level = 0;
			symbol_table[symbol_table_id].addr = 0;
			symbol_table_id++;

		// commasym
		}while(token == 17);
		
	}
	// check if line ends with semicolon
	if(token == 18){
		return 1;
	}
	else{ // if next token is not a comma or semicolon this is an error
		printf("Error: constant and variable declarations must be followed by a semicolon\n");
		exit(0);
	}	
}

// returns the number of variables declared
int varDeclaration(char token, char tokens[], int numVars){
	// varsym
	if(token == 29){
		do{
			numVars++;
			int identNameId =0;
			char identName[500] = {};

			token = getNextToken(tokens);
		
			// check for identifier
			if(token != 2){
				printf("Error: const, var, procedure, call and read keywords must be followed by identifier\n");
				exit(0);
			}
			// check if already in symbol table
			if(symbolTableCheck(token, tokens) != -1){
				printf("Error: symbol name has already beeen declared\n");
				exit(0);
			}
	
			// get identifier name
			strcpy(identName, getIdentifier(identName, tokens));

			// Add to symbol table
			symbol_table[symbol_table_id].kind = 2;
			strcpy(symbol_table[symbol_table_id].name, identName);
			symbol_table[symbol_table_id].val = 0;
			symbol_table[symbol_table_id].level = 0;
			symbol_table[symbol_table_id].addr = numVars + 3;
			symbol_table_id++;

			token = getNextToken(tokens);
		// commasym		
		}while(token == 17);
	}
	// check if line ends with semicolon
	if(token == 18){
		return numVars;
	}
	else{ // if next token is not a comma or semicolon this is an error
		printf("Error: constant and variable declarations must be followed by a semicolon\n");
		exit(0);
	}
}

// get factor
int factor(char token, char tokens[]){
	int symId=0;

	//identsym
	if(token == 2){
		symId = symbolTableCheck(token, tokens);
		
		if(symId == -1){
			printf("Error: undeclared symbol \n");
			exit(0);
		}
		if (symbol_table[symId].kind == 1){ // const
			emit(lineNum, "LIT", 0, symbol_table[symId].val);
			lineNum++;
		}
		else{ //var
			emit(lineNum, "LOD", 0, symbol_table[symId].addr);
			lineNum++;
		}
		// skips over identifier
		while(tokens[tokensId] > 47){ tokensId++; }
		
		token = getNextToken(tokens);

		return token;
	}
	//if number
	else if(token == 3){
		char constNum[6] = {};

		strcpy(constNum, getIdentifier(constNum,tokens));

		int constNumberDigit = atoi(constNum); // converts to int from char

		token = getNextToken(tokens);

		emit(lineNum, "LIT", 0, constNumberDigit);
		lineNum++;
	}
	//if left parenthesis
	else if(token == 15){
		token = getNextToken(tokens);

		token = expression(token, tokens);

		//right parenthesis
		if(token != 16){
			printf("Error: right parenthesis must follow left parenthesis\n");
			exit(0);
		}

		token = getNextToken(tokens);
	}
	else{
		printf("Error: arithmetic equations must contain operands, parenthesis, numbers or symbols\n");
		exit(0);
	}

	return token;
}

// get term
int term(char token, char tokens[]){
	token = factor(token, tokens);

	// while token is multsym, slashsym, or modsym
	while(token == 6 || token == 7 || token == 1){
		//if multsym
		if(token == 6){
			token = getNextToken(tokens);

			token = factor(token, tokens);

			// MUL
			emit(lineNum, "OPR", 0, 4);
			lineNum++;
		}
		// if slashsym
		else if(token == 7){
			token = getNextToken(tokens);

			token = factor(token, tokens);

			//DIV
			emit(lineNum, "OPR", 0, 5);
			lineNum++;
		}
		else{ // else its a modsym
			token = getNextToken(tokens);

			token = factor(token, tokens);

			//MOD
			emit(lineNum, "OPR", 0, 7);
			lineNum++;
		}
	}
	return token;
}

// get expression
int expression(char token, char tokens[]){
	// if minussym
	if(token == 5){
		token = getNextToken(tokens);

		token = term(token, tokens);

		//SUB
		emit(lineNum, "OPR", 0, 3);
		lineNum++;

		// while plusym or minussym
		while(token == 4 || token == 5){
			if(token == 4){
				// skips over identifier
				while(token > 47){token = getNextToken(tokens);}

				token = term(token, tokens);

				//ADD
				emit(lineNum, "OPR", 0, 2);
				lineNum++;
			}
			else{
				//skips over identifier
				while(token > 47){token = getNextToken(tokens);}

				token = term(token, tokens);

				//SUB
				emit(lineNum, "OPR", 0, 3);
				lineNum++;
			}
		}
	}
	else{
		//plussym
		if(token == 4){
			// skips over identifier
			do{tokensId++;}while(tokens[tokensId] > 47);
			token = getNextToken(tokens);
			return token;
		}
		token = term(token, tokens);
		while(token == 4 || token == 5){
			if(token == 4){
				token = getNextToken(tokens);

				token = term(token, tokens);

				//ADD
				emit(lineNum, "OPR", 0, 2);
				lineNum++;
			}
			else{
				token = getNextToken(tokens);

				token = term(token, tokens);

				//SUB
				emit(lineNum, "OPR", 0, 3);
				lineNum++;
			}
		}
	}
	return token;
}

// get condition
int condition(char token, char tokens[]){
	// oddsym
	if(token == 8){
		token = getNextToken(tokens);

		token = expression(token, tokens);

		//ODD
		emit(lineNum, "OPR", 0, 6);
		lineNum++;
	}
	else{
		token = expression(token, tokens);

		// eqlsym
		if(token == 9){
			token = getNextToken(tokens);

			token = expression(token, tokens);

			//EQL
			emit(lineNum, "OPR", 0, 8);
			lineNum++;
		}
		// neqsym
		else if(token == 10){
			token = getNextToken(tokens);

			token = expression(token, tokens);

			//NEQ
			emit(lineNum, "OPR", 0, 9);
			lineNum++;
		}
		// lessym
		else if(token == 11){
			token = getNextToken(tokens);

			token = expression(token, tokens);

			// NEQ
			emit(lineNum, "OPR", 0, 10);
			lineNum++;
		}
		// leqsym
		else if(token == 12){
			token = getNextToken(tokens);

			token = expression(token, tokens);

			//NEQ
			emit(lineNum, "OPR", 0, 11);
			lineNum++;
		}
		// gtrsym
		else if(token == 13){
			token = getNextToken(tokens);

			token = expression(token, tokens);

			//GTR
			emit(lineNum, "OPR", 0, 12);
			lineNum++;
		}
		// geqsym
		else if(token == 14){
			token = getNextToken(tokens);

			token = expression(token, tokens);

			//GEQ
			emit(lineNum, "OPR", 0, 13);
			lineNum++;
		}
		else{
			printf("Error: condition must contain comparison operator\n");
			exit(0);
		}
	}
	return token;
}

// get statement
int statement(char token, char tokens[]){
	int symId = 0;
	
	// identsym
	if(token == 2){
		symId = symbolTableCheck(token, tokens);
		if(symId == -1){
			printf("Error: undeclared symbol\n");
			exit(0);
		}
		// not a var
		if(symbol_table[symId].kind != 2){
			printf("Error: only variable values may be altered\n");
			exit(0);
		}

		// Don't need to save the identifier so we skip over it
		while(tokens[tokensId] > 47){ tokensId++; }

		token = getNextToken(tokens);

		// becomessym
		if(token != 20){
			printf("Error: assignment statements must use := \n");
			exit(0);
		}

		token = getNextToken(tokens);

		token = expression(token, tokens);

		emit(lineNum, "STO", 0, symbol_table[symId].addr);

		while(token > 47){token = getNextToken(tokens);}

		lineNum++;
		return token;
	}
	// beginsym
	if(token == 21){
		do{
			token = getNextToken(tokens);

			token = statement(token, tokens);
		}while(token == 18); //semicolon
		
		// endsym
		if(token != 22){
			printf("Error: begin must be followed by end\n");
			exit(0);
		}
		token = tokens[tokensId]; 

		return token;
	}
	// ifsym
	if(token == 23){
		token = getNextToken(tokens);

		token = condition(token, tokens);

		int jpcId = lineNum;
		emit(lineNum, "JPC", 0, jpcId);
		lineNum++;

		//thensym
		if(token != 24){
			printf("Error: if must be followed by then\n");
			exit(0);
		}
		token = getNextToken(tokens);

		token = statement(token, tokens);

		code[jpcId].m = lineNum;

		return token;
	}
	//whilesym
	if(token == 25){
		token = getNextToken(tokens);

		int loopId = lineNum;

		token = condition(token, tokens);

		//dosym
		if(token != 26){
			printf("Error: while must be followed by do\n");
			exit(0);
		}

		token = getNextToken(tokens);
		
		int jpcId = lineNum;
		emit(lineNum, "JPC", 0, jpcId);
		lineNum++;
		
		token = statement(token, tokens);

		emit(lineNum, "JMP", 0, loopId);
		lineNum++;

		code[jpcId].m = lineNum;

		return token;
	}
	//readsym
	if(token == 32){
		int symId = 0;

		token = getNextToken(tokens);

		//identsym
		if(token != 2){
			printf("Error: const, var, procedure, call and read keywords must be followed by identifier\n");
			exit(0);
		}
		symId = symbolTableCheck(token, tokens);
		if(symId == -1){
			printf("Error: symbol name has already been declared\n");
			exit(0);
		}
		// not a var
		if(symbol_table[symId].kind != 2){
			printf("Error: only variable values may be altered\n");
			exit(0);
		}
		
		// skip over identifier
		do{ tokensId++; }while(tokens[tokensId] > 47);

		token = getNextToken(tokens);

		emit(lineNum, "SYS", 0, 2);
		lineNum++;
		emit(lineNum, "STO", 0, symbol_table[symId].addr);
		lineNum++;
		
		return token;
	}
	//writesym
	if(token == 31){
		token = getNextToken(tokens);

		token = expression(token, tokens);

		emit(lineNum, "SYS", 0, 1);
		lineNum++;

		return token;
	}
	return token;
}

int block(char token, char tokens[]){
	int numVars = 0;

	// constsym
	if(token == 28){
		constDeclaration(token, tokens);
		token = getNextToken(tokens);
	}

	//varsym
	if(token == 29){
		numVars = varDeclaration(token, tokens, numVars);
		emit(lineNum, "INC", 0, numVars+4);
		lineNum++;
	}
	
	token = getNextToken(tokens);

	token = statement(token, tokens); 

	return token;
}

int program(char token, char tokens[]){
	token = block(token, tokens);

	token = getNextToken(tokens);

	//periodsym
	if(token != 19){
		printf("Error: program must end with period\n");
		exit(0);
	}

	emit(lineNum, "SYS", 0, 3);

	return 1;
}

// main
void parser(char tokens[], int aflag, int vflag){
	char token;

	token = getNextToken(tokens);

	program(token, tokens);
	printf("\n");

	// if the assembly directive was inputted then output the assembly
	if( aflag == 1){ outputAssembly(); printf("\n");}
	
	vm(code, vflag); // send assembly to vm
}