// Created by Han Winchester
// Takes in input from lex.c
// outputs code and send to vm.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

#define MAX_SYMBOL_TABLE_SIZE 550

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
char token;
int procedureCount = 0;
int tokensId = 0; 
int symbol_table_id = 0;
int lineNum = 0; // also opcode, NOT LEXICAL GRAPHICAL LEVEL
int codeId = 0; 
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE] = {}; // initialize symbol table
instructions code[MAX_SYMBOL_TABLE_SIZE] = {};

// function header declaration
void expression(char tokens[], int lexLevel);
void block(char tokens[], int lexLevel, int param, int procedureId);
void statement(char tokens[], int lexLevel);

void outputAssembly(){
	int i = 0;

    printf("Generated Assembly: \n");
    printf("Line    OP   L   M\n");

	for(i=0;i<lineNum;i++){ printf("%d	%s   %d   %d\n", code[i].opcode, code[i].op, code[i].level, code[i].m); } // outputs the assembly line by line

	printf("\n");
}

// insert into code array to be used in vm.c
void emit(int opcode, char op[], int level, int m){
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
}

// parameters: list of tokens
// returns: the next token in the token list specified by tokensId
void getNextToken(char tokens[]){
	token = tokens[tokensId];
	tokensId++;
}

// iterate through identifier array to decrement o to not skip over tokens in tokens[tokensId] when saving the identifier name or number
void decrementTokenList(char tokens[], char identifier[]){
	
	int identifierId = 0;
	while(identifier[identifierId] != '\0'){
		identifierId++;
		tokensId--;
	}
	return;
}

// parameters: empty array for identifier name or number, list of tokens
// returns: the char pointer to identifier name or number
char* getIdentifier(char identName[], char tokens[]){
	int identNameId = 0;
	do{
		getNextToken(tokens);
		identName[identNameId] = token; // copy token from list to identName array
		identNameId++;
	}while(tokens[tokensId] > 47); // ascii value is greater than 47 (i.e. letter or number)
	return identName;
}

// parameters: attributes of symbol struct for symbol_table
// returns: adds these values to symbol_table at index symbol_table_id
void addToSymbolTable(int kind, char name[], int val, int level, int addr, int mark, int param){
	
	symbol_table[symbol_table_id].kind = kind;
	strcpy(symbol_table[symbol_table_id].name, name);
	symbol_table[symbol_table_id].val = val;
	symbol_table[symbol_table_id].level = level;
	symbol_table[symbol_table_id].addr = addr;
	symbol_table[symbol_table_id].mark = mark;
	symbol_table[symbol_table_id].param = param;
	symbol_table_id++;
}

// linear search through symbol table looking at name
// return index if found -1 if not
int symbolTableCheck(char tokens[], int lexLevel){
	char identifier[12] = {};
	int i = 0; int identifierId = 0;
	
	// Get identifier name
	strcpy(identifier, getIdentifier(identifier, tokens));

	decrementTokenList(tokens, identifier);

	// iterate through table array and check if name matches any entries
	for(i=0;i<=symbol_table_id;i++){
		if(!(strcmp(identifier, symbol_table[i].name)) && (symbol_table[i].level == lexLevel) && (symbol_table[i].mark == 0)){ // strcmp returns 0 if the strings match
			return i;
		}
	}
	return -1; // not found in symbol table
}

/* linear search through symbol table looking at name and kind
returns index for exact match of string and kind unmarked, -1 if not */
int symbolTableSearch(  char tokens[], int lexLevel, int kind){
	char identifier[12] = {};
	int i = 0; int identifierId = 0;

	// Get identifier name
	strcpy(identifier, getIdentifier(identifier, tokens));

	decrementTokenList(tokens, identifier);

	// iterate through table array backwards and check if name matches any entries
	for(i=symbol_table_id;i>=0;i--){
		if(!(strcmp(identifier, symbol_table[i].name)) && (symbol_table[i].level <= lexLevel) && (symbol_table[i].mark == 0) && (symbol_table[i].kind == kind)){ // strcmp returns 0 if the strings match
			return i;
		}
	}
	return -1;
}

// linear search through symbol table looking at procedure values return index if found, -1 if not
int findProcedure(int id){
	int i;
	for(i=0;i<=symbol_table_id;i++){
		if(symbol_table[i].kind == 3 && id == symbol_table[i].val){
			return i;
		}
	}
	return -1;
}

// starting from the end of the symbol table and looping backward if entry is unmarked, mark it
void mark(int count){
	int i;
	for(i=symbol_table_id;i>=0;i--){
		if(symbol_table[i].mark == 0){
			symbol_table[i].mark = 1;
			count--;
			if(count == 0){ return; }
		}
	}
	return;
}

int constDeclaration(char tokens[], int lexLevel){
	int numConst = 0;
	//constsym
	if(token == 28){
		do{
			numConst++;
			char identName[12] = {};
			//int identNameId = 0;

			getNextToken(tokens);

			// check for identsym
			if(token != 2){
				printf("Error: const, var, procedure, call and read keywords must be followed by identifier\n");
				exit(0);
			}
			if(symbolTableCheck(tokens, lexLevel) != -1){
				printf("Error: symbol name has already been declared\n");
				exit(0);
			}

			// Get identifier name
			strcpy(identName, getIdentifier(identName, tokens));
	
			getNextToken(tokens);

			// check for equal symbol
			if(token != 9){
				printf("Error: constants must be assigned with =\n");
				exit(0);
			}

			getNextToken(tokens);

			// check for number
			if(token !=  3){
				printf("Error: constants must be assigned an integer value\n");
				exit(0);
			}

			char number[6] = {};
			// gets the number for the constant
			strcpy(number, getIdentifier(number, tokens));

			int numberDigit = atoi(number); // convert from char array to int

			getNextToken(tokens);

			addToSymbolTable(1, identName, numberDigit, lexLevel, 0, 0, 0);

		// commasym
		}while(token == 17);
		// check if line ends with semicolon
		if(token != 18){
			printf("Error: constant and variable declarations must be followed by a semicolon\n");
			exit(0);
		}
		getNextToken(tokens);	
	}
	return numConst;
}

// returns the number of variables declared
int varDeclaration(char tokens[], int numVars, int lexLevel, int param){
	if(param == 1){
		numVars = 1;
	}
	else{
		numVars = 0;
	}
	
	// varsym
	if(token == 29){
		do{
			numVars++;
			char identName[12] = {};

			getNextToken(tokens);
		
			// check for identifier
			if(token != 2){
				printf("Error: const, var, procedure, call and read keywords must be followed by identifier\n");
				exit(0);
			}
			// check if already in symbol table
			if(symbolTableCheck(tokens, lexLevel) != -1){
				printf("Error: symbol name has already beeen declared\n");
				exit(0);
			}
	
			// get identifier name
			strcpy(identName, getIdentifier(identName, tokens));

			addToSymbolTable(2, identName, 0, lexLevel, numVars+3, 0, 0);

			getNextToken(tokens);
		// commasym		
		}while(token == 17);
		// check if line ends with semicolon
		if(token != 18){
			printf("Error: constant and variable declarations must be followed by a semicolon\n");
			exit(0);
		}
		getNextToken(tokens);
	}
	return numVars;
}

int procDeclaration(char tokens[], int lexLevel){
	int numProc = 0;
	int procId = 0;

	// procsym
	if(token == 30){
		do{
			char identName[12] = {};
			char identName2[12] = {};
			numProc++;
			getNextToken(tokens);
			//identsym
			if(token != 2){
				printf("Error: const, var, procedure, call, and read keywords must be followed by identifier\n"); 
				exit(0);
			}
			if(symbolTableCheck(tokens, lexLevel) != -1){
				printf("Error: symbol name has already been declared\n");
				exit(0);
			}

			// end of symbol table
			procId = symbol_table_id;

			// get identifier name
			strcpy(identName, getIdentifier(identName, tokens));

			addToSymbolTable(3, identName, procedureCount, lexLevel, 0, 0, 0);

			procedureCount++;

			getNextToken(tokens);
			
			//lparentsym
			if(token == 15){
				getNextToken(tokens);
				//identsym
				if(token != 2){
					printf("parameters may only be specified by an identifier \n");
					exit(0);
				}

				// get identifier name
				strcpy(identName2, getIdentifier(identName2, tokens));

				addToSymbolTable(2, identName2, 0, lexLevel+1, 3, 0, 0);
				symbol_table[procId].param = 1;

				getNextToken(tokens);

				//rparentsym
				if(token != 16){
					printf("Error: right parenthesis must follow left parenthesis\n"); 
					exit(0);
				}

				getNextToken(tokens);

				//semicolonsym
				if(token != 18){
					printf("Error: symbol declarations must be followed by a semicolon\n"); 
					exit(0);
				}

				getNextToken(tokens);

				block(tokens, lexLevel + 1, 1, procId);
			}
			else{
				//semicolonsym
				if(token != 18){
					printf("Error: symbol declarations must be followed by a semicolon\n"); 
					exit(0);
				}

				getNextToken(tokens);

				block(tokens, lexLevel+1, 0, procId);
			}
			
			if(strcmp(code[lineNum-1].op, "OPR") != 0 && code[lineNum-1].m != 0){
				emit(lineNum, "LIT", 0, 0);
				lineNum++;
				emit(lineNum, "OPR", 0, 0);
				lineNum++;
			}

			// semicolonsym
			if(token != 18){
				printf("Error: symbol declarations must be followed by a semicolon\n"); 
				exit(0);
			}
			getNextToken(tokens);
			
		}while(token == 30);
	}
	return numProc;
}

// get factor
void factor(char tokens[], int lexLevel){
	int symIdV=0; int symIdC = 0;

	//identsym
	if(token == 2){
		symIdV = symbolTableSearch(tokens, lexLevel, 2);
		symIdC = symbolTableSearch(tokens, lexLevel, 1);
		if(symIdV == -1 && symIdC == -1){
			printf("Error: competing symbol declarations at the same level\n");
			exit(0);
		}
		else if(symIdC == -1 || (symIdV != -1 && symbol_table[symIdV].level > symbol_table[symIdC].level)){
			emit(lineNum, "LOD", lexLevel - symbol_table[symIdV].level, symbol_table[symIdV].addr);
			lineNum++;
		}
		else{
			emit(lineNum, "LIT", lexLevel, symbol_table[symIdC].val);
			lineNum++;
		}

		// skips over identifier
		while(tokens[tokensId] > 47){ tokensId++; }
		
		getNextToken(tokens);
	}
	//if number
	else if(token == 3){
		char constNum[6] = {};

		strcpy(constNum, getIdentifier(constNum,tokens));

		int constNumberDigit = atoi(constNum); // converts to int from char

		getNextToken(tokens);

		emit(lineNum, "LIT", 0, constNumberDigit);
		lineNum++;
	}
	//if left parenthesis
	else if(token == 15){
		getNextToken(tokens);

		expression(tokens, lexLevel);

		//right parenthesis
		if(token != 16){
			printf("Error: right parenthesis must follow left parenthesis\n");
			exit(0);
		}

		getNextToken(tokens);
	}
	// if callsym
	else if(token == 27){
		statement(tokens, lexLevel);
	}
	else{
		printf("Error: arithmetic equations must contain operands, parenthesis, numbers or symbols\n");
		exit(0);
	}
}

// get term
void term(  char tokens[], int lexLevel){
	factor(tokens, lexLevel);

	// while token is multsym, slashsym, or modsym
	while(token == 6 || token == 7 || token == 1){
		//if multsym
		if(token == 6){
			getNextToken(tokens);

			factor(tokens, lexLevel);

			// MUL
			emit(lineNum, "OPR", 0, 4);
			lineNum++;
		}
		// if slashsym
		else if(token == 7){
			getNextToken(tokens);

			factor(tokens, lexLevel);

			//DIV
			emit(lineNum, "OPR", 0, 5);
			lineNum++;
		}
		else{ // else its a modsym
			getNextToken(tokens);

			factor(tokens, lexLevel);

			//MOD
			emit(lineNum, "OPR", 0, 7);
			lineNum++;
		}
	}
}

// get expression
void expression(  char tokens[], int lexLevel){
	// if minussym
	if(token == 5){
		getNextToken(tokens);

		term(tokens, lexLevel);

		//SUB
		emit(lineNum, "OPR", 0, 3);
		lineNum++;

		// while plusym or minussym
		while(token == 4 || token == 5){
			if(token == 4){
				// skips over identifier
				while(tokens[tokensId] > 47){ tokensId++; }
		
				getNextToken(tokens);

				term(tokens, lexLevel);

				//ADD
				emit(lineNum, "OPR", 0, 2);
				lineNum++;
			}
			else{
				// skips over identifier
				while(tokens[tokensId] > 47){ tokensId++; }
		
				getNextToken(tokens);

				term(tokens, lexLevel);

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
			while(tokens[tokensId] > 47){ tokensId++; }
		
			getNextToken(tokens);
			
		}
		term(tokens, lexLevel);
		while(token == 4 || token == 5){
			if(token == 4){
				getNextToken(tokens);

				term(tokens, lexLevel);

				//ADD
				emit(lineNum, "OPR", 0, 2);
				lineNum++;
			}
			else{
				getNextToken(tokens);

				term(tokens, lexLevel);

				//SUB
				emit(lineNum, "OPR", 0, 3);
				lineNum++;
			}
		}
	}
}

// get condition
void condition(char tokens[], int lexLevel){
	// oddsym
	if(token == 8){
		getNextToken(tokens);

		expression(tokens, lexLevel);

		//ODD
		emit(lineNum, "OPR", 0, 6);
		lineNum++;
	}
	else{
		expression(tokens, lexLevel);

		// eqlsym
		if(token == 9){
			getNextToken(tokens);

			expression(tokens, lexLevel);

			//EQL
			emit(lineNum, "OPR", 0, 8);
			lineNum++;
		}
		// neqsym
		else if(token == 10){
			getNextToken(tokens);

			expression(tokens, lexLevel);

			//NEQ
			emit(lineNum, "OPR", 0, 9);
			lineNum++;
		}
		// lessym
		else if(token == 11){
			getNextToken(tokens);

			expression(tokens, lexLevel);

			// NEQ
			emit(lineNum, "OPR", 0, 10);
			lineNum++;
		}
		// leqsym
		else if(token == 12){
			getNextToken(tokens);

			expression(tokens, lexLevel);

			//NEQ
			emit(lineNum, "OPR", 0, 11);
			lineNum++;
		}
		// gtrsym
		else if(token == 13){
			getNextToken(tokens);

			expression(tokens, lexLevel);

			//GTR
			emit(lineNum, "OPR", 0, 12);
			lineNum++;
		}
		// geqsym
		else if(token == 14){
			getNextToken(tokens);

			expression(tokens, lexLevel);

			//GEQ
			emit(lineNum, "OPR", 0, 13);
			lineNum++;
		}
		else{
			printf("Error: condition must contain comparison operator\n");
			exit(0);
		}
	}
}

// get statement
void statement(char tokens[], int lexLevel){
	int symId = 0;

	// identsym
	if(token == 2){
		symId = symbolTableSearch(tokens, lexLevel, 2);
		if(symId == -1){
			printf("Error: undeclared variable or constant in equation\n");
			exit(0);
		}
		// not a var
		if(symbol_table[symId].kind != 2){
			printf("Error: only variable values may be altered\n");
			exit(0);
		}

		// skips over identifier
		while(tokens[tokensId] > 47){ tokensId++; }
		
		getNextToken(tokens);

		// becomessym
		if(token != 20){
			printf("Error: assignment statements must use := \n");
			exit(0);
		}

		getNextToken(tokens);

		expression(tokens, lexLevel);
		
		emit(lineNum, "STO", lexLevel - symbol_table[symId].level, symbol_table[symId].addr);

		while(token > 47){getNextToken(tokens);}

		lineNum++;
		
	}
	// callsym
	if(token == 27){

		getNextToken(tokens);
		// identsym
		if(token != 2){
			printf("const, var, procedure, call, and read keywords must be followed by identifier\n");
			exit(0);
		}
		symId = symbolTableSearch(tokens, lexLevel, 3);
		if(symId == -1){
			printf("Error: undeclared procedure for call\n");
			exit(0);
		}

		do{
			getNextToken(tokens);
		}
		while(token>47);

		// lparentsym
		if(token == 15){
			getNextToken(tokens);
			if(symbol_table[symId].param != 1){
				printf("Error: parameters must be declared\n"); 
				exit(0);
			}
			expression(tokens, lexLevel);
			//rparentsym
			if(token != 16){
				printf("Error: right parenthesis must follow left parenthesis\n"); 
				exit(0);
			}
			getNextToken(tokens);
		}
		else{
			emit(lineNum, "LIT", 0, 0);
			lineNum++;
		}

		emit(lineNum, "CAL", lexLevel - symbol_table[symId].level, symbol_table[symId].addr);
		lineNum++;
	}

	//returnsym
	if(token == 34){
		if(lexLevel == 0){
			printf("Error: cannot return from main\n");
			exit(0);
		}
		getNextToken(tokens);

		// lparentsym
		if(token == 15){
			getNextToken(tokens);
			expression(tokens, lexLevel);
			emit(lineNum, "OPR", 0, 0);
			lineNum++;
			// rparentsym
			if(token != 16){
				printf("Error: right parenthesis must follow left parenthesis\n");
			}
			getNextToken(tokens);
		}
		else{
			emit(lineNum, "LIT", 0, 0);
			lineNum++;
			emit(lineNum, "OPR", 0, 0);
			lineNum++;
		}
	}

	// beginsym
	if(token == 21){
		do{
			getNextToken(tokens);
			
			statement(tokens, lexLevel);

		}while(token == 18); //semicolon

		// endsym
		if(token != 22){
			printf("Error: begin must be followed by end\n");
			exit(0);
		}
		
		getNextToken(tokens);
		
	}
	// ifsym
	if(token == 23){
		getNextToken(tokens);

		condition(tokens, lexLevel);

		int jpcId = lineNum;
		emit(lineNum, "JPC", 0, jpcId);
		lineNum++;

		//thensym
		if(token != 24){
			printf("Error: if must be followed by then\n");
			exit(0);
		}
		getNextToken(tokens);

		statement(tokens, lexLevel);

		// elsesym
		if(token == 33){
			getNextToken(tokens);
			int jmpId = lineNum;
			emit(lineNum, "JMP", lexLevel, jmpId);
			lineNum++;
			code[jpcId].m = lineNum;
			statement(tokens, lexLevel);
			code[jmpId].m = lineNum;
		}
		else{
			code[jpcId].m = lineNum;
		}
	}
	//whilesym
	if(token == 25){
		getNextToken(tokens);

		int loopId = lineNum;

		condition(tokens, lexLevel);

		//dosym
		if(token != 26){
			printf("Error: while must be followed by do\n");
			exit(0);
		}

		getNextToken(tokens);
		
		int jpcId = lineNum;
		emit(lineNum, "JPC", 0, jpcId);
		lineNum++;
		
		statement(tokens, lexLevel);

		emit(lineNum, "JMP", lexLevel, loopId);
		lineNum++;

		code[jpcId].m = lineNum;
	}
	//readsym
	if(token == 32){
		int symId = 0;

		getNextToken(tokens);

		//identsym
		if(token != 2){
			printf("Error: const, var, procedure, call and read keywords must be followed by identifier\n");
			exit(0);
		}
		symId = symbolTableSearch(tokens, lexLevel, 2);
		if(symId == -1){
			printf("Error: undeclared variable or constant in equation\n");
			exit(0);
		}
		// not a var
		if(symbol_table[symId].kind != 2){
			printf("Error: only variable values may be altered\n");
			exit(0);
		}
		
		// skips over identifier
		while(tokens[tokensId] > 47){ tokensId++; }
		
		getNextToken(tokens);

		emit(lineNum, "SYS", 0, 2);
		lineNum++;
		emit(lineNum, "STO", lexLevel - symbol_table[symId].level, symbol_table[symId].addr);
		lineNum++;	
	}
	//writesym
	if(token == 31){
		getNextToken(tokens);

		expression(tokens, lexLevel);

		emit(lineNum, "SYS", 0, 1);
		lineNum++;
	}
}

void block(char tokens[], int lexLevel, int param, int procedureId){
	int numVars = 0; int c = 0; int p = 0;
	
	// constsym
	c = constDeclaration(tokens, lexLevel);
	
	//varsym
	numVars = varDeclaration(tokens, numVars, lexLevel, param);
		
	// procsym
	p = procDeclaration(tokens, lexLevel);
	
	symbol_table[procedureId].addr = lineNum;

	emit(lineNum, "INC", 0, numVars+4);
	lineNum++;

	statement(tokens, lexLevel); 
	
	mark(c+numVars+p);
}

void program(char tokens[]){
	int numProc = 1;

	emit(lineNum, "JMP", 0, 0);
	lineNum++;

	int i;
	for(i=0;i<=strlen(tokens);i++){
		//procsym
		if(tokens[i] == 30){
			numProc++;
			emit(lineNum, "JMP", 0, 0);
			lineNum++;
		}
	}

	addToSymbolTable(3, "main", 0, 0, 0, 0, 0);

	procedureCount++;

	block(tokens, 0, 0, 0);

	//periodsym
	if(token != 19){
		printf("Error: program must end with period\n");
		exit(0);
	}
	int j;
	for(j=0;j< numProc;j++){
		code[j].m = symbol_table[findProcedure(j)].addr;
	}

	emit(lineNum, "SYS", 0, 3);
	lineNum++;
}

// main
void parser(char tokens[], int aflag, int vflag){
	
	getNextToken(tokens);

	program(tokens);
	printf("\n");
	
	// if the assembly directive was inputted then output the assembly
	if( aflag == 1){ outputAssembly(); printf("\n");}
	
	vm(code, vflag); // send assembly to vm
}