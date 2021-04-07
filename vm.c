// Created by Han Winchester
// takes in input from parser.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compiler.h"

#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 100 // Will never be given an input file with more than this number of lines

// Instruction Register
typedef struct instruction{
	int OP; // operation code
	int L; // lexicographical
	int M; // depends on opcode
}instruction;

// find a variable(base) in an activation record some L levels down
int base(int stack[], int level, int BP){
	int base = BP;
	int counter = level;
	while(counter > 0){
		base = stack[base];
		counter--;
	}
	return base;
}

void vm(struct instructions codeText[], int vflag){
	// initial values for PM/0 CPU registers
	int SP = -1, BP = 0, PC = 0;
	int i = 0, lineNum = 0, halt = 0, oldBP = 0; // lineNum is used to store the PC before incrementing it by 1 in the fetch cycle. Halt == 0 to make sure the program continues
	instruction IR;	// instruction register
	int stack[MAX_STACK_HEIGHT] = {};// stack can only get as high as this value

	/*Output header*/
	if(vflag == 1){ 
		printf("		PC	BP	SP	stack\n");
		printf("Initial values: %d	%d	%d\n", PC, BP, SP);
	}
	
	// Read the code from input
	while(halt == 0){ // halt == 0 is continue; halt == 1 is end
		int i = 0;
		/*Fetch Cycle*/
		// store data from the codegen and parser in the instruction register
		IR.OP = codeText[PC].opNum;
		IR.L = codeText[PC].level;
		IR.M = codeText[PC].m;
		
		lineNum = PC; // lineNum is to store the PC before incrementing it in order to get the correct value for the current instruction
		PC++; // increment PC to go to next instruction
				
		/*Execution Cycle*/ 
		switch(IR.OP){
			case 1: // LIT loads a constant value M onto stack
				if(vflag == 1){ printf("%d ", lineNum); }
				SP++;
				stack[SP] = IR.M;

				if(vflag == 1){ printf("LIT%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
				
				for(i=0;i<=SP;i++){ //for loop to print the contents of the stack
					if(i == BP && BP != 0){
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records 
					}
					if(i ==  oldBP && oldBP !=0 && oldBP != BP){ // essentially after the first if statement above then check for the old BP 
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records 
					}
					if(vflag == 1){ printf("%d ", stack[i]); } 
					
				}
				if(vflag == 1){ printf("\n"); } 
				break;
				
			case 2: // OPR Check what value M is to do arithmetic operations
				switch(IR.M){
					case 0: //RTN
						if(vflag == 1){ printf("%d ", lineNum); }
						stack[BP - 1] = stack[SP];
						SP = BP - 1; // change stack pointer
						BP = stack[SP + 2]; //BP gets whatever value is at this position in the stack
						PC = stack[SP + 3];
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){ // check where the base pointer is to print the bar |
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
					
					case 1: // NEG
						if(vflag == 1){ printf("%d ", lineNum); }
						stack[SP] = -1 * stack[SP]; //make whatever value is at this position of the stack negative
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 2: // ADD
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1; // decrease the stack pointer
						stack[SP] = stack[SP] + stack[SP + 1]; // add two elements of the stack together

						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); }
							}
							if(vflag == 1){  printf("%d ", stack[i]);  }
							
						}
						if(vflag == 1){ printf("\n"); }
								
						break;
					
					case 3: // SUB
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] - stack[SP + 1]; // subtract two elements of the stack together
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 4: // MUL
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] * stack[SP + 1]; // multiply two elements of the stack together
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 5: // DIV
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] / stack[SP + 1]; // divide two elements of the stack together
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 6: // ODD
						if(vflag == 1){ printf("%d ", lineNum); }
						stack[SP] = stack[SP] % 2; // returns the remainder, returns zero if even
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 7: // MOD						
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] % stack[SP + 1]; // gets the remainder of two elements in the stack
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}	
											
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 8: // EQL
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] == stack[SP + 1]; // checks if two elements in the stack are equal
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 9: // NEQ
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] =  stack[SP] != stack[SP + 1]; // checks if two elements in the stack are not equal
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 10: // LSS
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] < stack[SP + 1]; // checks if one element in the stack is less than another
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 11: // LEQ
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] <= stack[SP + 1]; // checks if one element in the stack is less than or equal to another
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 12: // GTR
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] > stack[SP + 1]; // checks if an element in the stack is greater than another
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 13: // GEQ
						if(vflag == 1){ printf("%d ", lineNum); }
						SP = SP - 1;
						stack[SP] = stack[SP] >= stack[SP + 1]; // checks if an element in the stack is greater than or equal to another
						if(vflag == 1){ printf("OPR%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" ");}
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					default: // error
						printf("There was an error in OPR(case 2) switch, Couldn't read the M element");
						break;
				}
				break;
				/* END OPR SWITCH*/
			
			case 3: // LOD push from location at offset M in fram L levels down
				if(vflag == 1){ printf("%d ", lineNum); }
				SP = SP + 1;
				stack[SP] = stack[base(stack, IR.L, BP) + IR.M];
				if(vflag == 1){ printf("LOD%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
				
				for(i=0;i<=SP;i++){
					if(i == BP && BP != 0){
						if(vflag == 1){ printf(" | "); }
					}
					if(i ==  oldBP && oldBP !=0 && oldBP != BP){
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
					}
					if(vflag == 1){ printf("%d ", stack[i]); }
				}
				if(vflag == 1){ printf("\n"); }
				break;
				
			case 4: // STO store in location at offset M in fram L levels down
				if(vflag == 1){ printf("%d ", lineNum); }
				stack[base(stack, IR.L, BP) + IR.M] = stack[SP];
				SP = SP - 1;
				if(vflag == 1){ printf("STO%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
				
				for(i=0;i<=SP;i++){
					if(i == BP && BP != 0){
						if(vflag == 1){ printf(" | "); }
					}
					if(i ==  oldBP && oldBP !=0 && oldBP != BP){
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
					}
					if(vflag == 1){ printf("%d ", stack[i]); }
				}
				if(vflag == 1){ printf("\n"); }
				break;
				
			case 5: // CAL call procedure at M, Generates new AR
				if(vflag == 1){ printf("%d ", lineNum); }
				stack[SP + 1] = base(stack, IR.L, BP); // Static link SL
				stack[SP + 2] = BP;					// Dynamic Link DL
				stack[SP + 3] = PC;					// Return Address RA
				stack[SP + 4] = stack[SP];			// Parameter P
				oldBP = BP;	// get oldBP to remember where the base pointers are for the Activation Records to print | to separate the AR's
				BP = SP + 1;
				PC = IR.M;
				if(vflag == 1){ printf("CAL%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
				
				for(i=0;i<=SP;i++){
					if(i == BP && BP != 0){
						if(vflag == 1){ printf(" | "); }
					}
					if(i ==  oldBP && oldBP !=0 && oldBP != BP){
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
					}
					if(vflag == 1){ printf("%d ", stack[i]); }
				}
				if(vflag == 1){ printf("\n"); }
				break;
				
			case 6: // INC allocate M locals first three are SL DL RA
				if(vflag == 1){ printf("%d ", lineNum); }
				SP = SP + IR.M;
				if(vflag == 1){ printf("INC%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
				
				for(i=0;i<=SP;i++){
					if(i == BP && BP != 0){
						if(vflag == 1){ printf(" | "); }
					}
					if(i ==  oldBP && oldBP !=0 && oldBP != BP){
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
					}
					if(vflag == 1){ printf("%d ", stack[i]); }
				}
				if(vflag == 1){ printf("\n"); }
				break;
				
			case 7: // JMP 
				if(vflag == 1){ printf("%d ", lineNum); }
				PC = IR.M; // jump to another instruction at M
				if(vflag == 1){ printf("JMP%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
				
				for(i=0;i<=SP;i++){
					if(i == BP && BP != 0){
						if(vflag == 1){ printf(" | "); }
					}
					if(i ==  oldBP && oldBP !=0 && oldBP != BP){
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
					}
					if(vflag == 1){ printf("%d ", stack[i]); }
				}
				if(vflag == 1){ printf("\n"); }
				break;
				
			case 8: // JPC
				if(vflag == 1){ printf("%d ", lineNum); }
				if(stack[SP] == 0){
					PC = IR.M; // jump to code at M if top of stack is equal to zero
				}
				SP = SP - 1;
				if(vflag == 1){ printf("JPC%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
				
				for(i=0;i<=SP;i++){
					if(i == BP && BP != 0){
						if(vflag == 1){ printf(" | "); }
					}
					if(i ==  oldBP && oldBP !=0 && oldBP != BP){
						if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
					}
					if(vflag == 1){ printf("%d ", stack[i]); }
				}
				if(vflag == 1){ printf("\n"); }
				break;
				
			case 9: // SYS 
				switch(IR.M){
					case 1:
						printf("Top of Stack Value: %d\n", stack[SP]); // write top stack element to screen
						SP = SP - 1;
						if(vflag == 1){ printf("%d ", lineNum); }
						if(vflag == 1){ printf("SYS%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP); printf(" "); }
						
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 2: // read in input from user and store at top of stack
						SP = SP + 1;
						printf("Please Enter an Integer: "); 
						scanf("%d", &stack[SP]);
						printf("\n");
						
						if(vflag == 1){ 
							printf("%d ", lineNum); 
							printf("SYS%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP);
							printf(" ");
						}
						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
						}
						if(vflag == 1){ printf("\n"); }
						break;
						
					case 3:
						halt = 1; // halt flag at zero is end of program
						if(vflag == 1){ 
							printf("%d ", lineNum); 						
							printf("SYS%3d%3d	%d	%d	%d	", IR.L, IR.M, PC, BP, SP);
							printf(" ");
						}

						for(i=0;i<=SP;i++){
							if(i == BP && BP != 0){
								if(vflag == 1){ printf(" | "); }
							}
							if(i ==  oldBP && oldBP !=0 && oldBP != BP){
								if(vflag == 1){ printf(" | "); } // prints bar if to separate the activation records
							}
							if(vflag == 1){ printf("%d ", stack[i]); }
							
						}
						if(vflag == 1){ printf("\n"); }
						
						break;
					
					default: // error
						printf("Error in case 9, couldn't read M. Please give a number between 1 and 3 for M");						
						break;
				}
				/*END SYS SWITCH */
				break;
			
			default: // error
				printf("There was an error in the main switch, Couldn't read OP code");
				break;
		} /*END SWITCH */
	}/*END WHILE*/
}