// Created by Han Winchester
// takes in input from input file
// outputs to parser.c

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

// Numbers can be a maximum of 5 digits in length.
#define cmax 11 // Identifiers can be a maximum of 11 characters in length.

// use to easily tokenize each symbol
typedef enum { modsym = 1, identsym, numbersym, plussym, minussym,multsym,  
slashsym, oddsym, eqsym, neqsym, lessym, leqsym,gtrsym, geqsym, lparentsym, 
rparentsym, commasym, semicolonsym,periodsym, becomessym, beginsym, endsym, 
ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym, writesym,readsym , 
elsesym, returnsym 
} token_type;


// for token list; these are global 
char tokenString[550] = {};
int p = 0;

// Function to output the token list
int outputTokenList(int lflag){

	if(lflag == 0){ return 1; } // if lflag isn't detected dont print

	printf("\n");
	printf("Token List:\n");
	int h=0;

	//output the token list
	while(tokenString[h] != '\0'){ // while not at end of list
		if(tokenString[h] > 47){ // greater than 47 because ascii code below this are the special symbols which doesn't need to be converted
			if(tokenString[h] >= 48 && tokenString[h] <= 57){
				char(output) = tokenString[h]; // convert from ascii code to ascii char
				printf("%c", output);
			}
			else{
				char(output) = tokenString[h]; // convert from ascii code to ascii char
				printf("%c", output);
			}
		}
		else{
			printf(" ");
			int(output) = tokenString[h]; // else output the token number
			printf("%d ", output); 
		}
		h++;
	}
	printf(" \n");
	printf(" \n");
	return 1;
}

// Function to read in the comment to ignore it
// Parameter Values on 1st call: currentChar == '/'   nextChar == '*'
// Parameter Values on recursive calls: currentChar == '<some character within comment>'   nextChar == '*'
// On return: currentChar == '/'       nextChar == '<next character right outside comment>'
int isComment(char currentChar, char nextChar, FILE* inputFile){ 
	
	nextChar = fgetc(inputFile);

	while(nextChar != '*'){ // keep reading in char until we reach a '*'
		nextChar = fgetc(inputFile);
	}

	currentChar = fgetc(inputFile); // currentChar gets next char from input if nextChar == '*'

	if(currentChar == '/'){ // check if currentChar == '/' to indicate the end of comment
		nextChar = fgetc(inputFile); // gets the next character after the '/' as this is out of the comment and will get put back in input file when this function returns. This char becomes currentChar in main
		return 1;
	}
	else{
		isComment(currentChar, nextChar, inputFile); // else call same function again to keep reading in char until end of comment
	}
}


// should have already checked for errors or inconsistencies before this function is called. The string parameter passed in will either be an identifier or reserved word
// This function takes in a valid array of letters and outputs either the reserved word or an identifier
int letterToken(char letter[], char *reservedWords[], int lflag){
	token_type token;
	int i, j=0,k=0;

	for(i=0;i<=15;i++){ // iterate through the reserved words array to check if the string read in is a match

		/* REMEMBER TO CHANGE THIS BACK TO 14 FOR HW4	*/
		
		if(i>10){ // if not in the reserved words array then it is an identifier
			token = identsym; // get it's respective token
			if(lflag == 1){ printf("	%s	%d\n", letter, token); }
			tokenString[p] = token; // add token to token list array
			p++;
			while(letter[k] != '\0'){ // while not at end of array
				tokenString[p] = letter[k]; // add the letter to token list array
				p++;
				k++;
			}
			break;
		}
		if(!(strcmp(letter, reservedWords[i]))){ // check to see if the string is a reserved word; strcmp returns 0 if the strings match
			switch(i){ // if it is a reserved word then assign the token and output
				case 0:
					token = constsym;// get it's respective token
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;// add token to token list array
					p++;
					break;
				case 1:
					token = varsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				/* REMEMBER TO ALSO CHANGE BACK THE NUMBERS FOR THE FOLLOWING CASES FOR HW4
				case 2:
					token = procsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				case 3:
					token = callsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;*/
				case 2:
					token = ifsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				case 3:
					token = thensym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				/*case 6:
					token = elsesym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;*/
				case 4:
					token = whilesym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				case 5:
					token = dosym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				case 6:
					token = readsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				case 7:
					token = writesym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				case 8:
					token = oddsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				/*case 9:
					token = returnsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;*/
				case 9:
					token = beginsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				case 10:
					token = endsym;
					if(lflag == 1){ printf("	%s	%d\n", letter, token); }
					tokenString[p] = token;
					p++;
					break;
				default:
					printf("There was an error in the letter strcmp switch.");
					break;
			}
			break; // break from for loop
		}
	}
	return 1;
}

// should have already checked for errors or inconsistencies before this function is called.
// This function takes in a valid char array of numbers and tokenizes it
int numberToken(char number[], int lflag){
	int i, j=0;
	token_type token;
	token = numbersym;// get it's respective token
	if(*number == '\0'){ // if the array of numbers is empty then don't output it
		return 1; 
	}
	if(lflag == 1){ printf("	%s	%d\n", number, token); }
	tokenString[p] = token;// add token to token list array
	p++;
	while(number[j] != '\0'){ // while not at end of array
		tokenString[p] = number[j]; // add the numbers in the array to the token list
		p++;
		j++;
	}
	return 1;
}

// should have already checked for errors or inconsistencies before this function is called.
// Function takes in a valid symbol and tokenizes it
int specialSymbolToken(char symbol, char nextSymbol, int lflag){
	token_type token;

	// assign token then output
	switch(symbol){
		case '+':
			token = plussym;// get it's respective token
			if(lflag == 1){ printf("	+	%d\n", token); }
			tokenString[p] = token;// add token to token list array
			p++;
			break;

		case '-':
			token = minussym;
			if(lflag == 1){ printf("	-	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case '*':
			token = multsym;
			if(lflag == 1){ printf("	*	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case '/':
			token = slashsym;
			if(lflag == 1){ printf("	/	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case '(':
			token = lparentsym;
			if(lflag == 1){ printf("	(	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case ')':
			token = rparentsym;
			if(lflag == 1){ printf("	)	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case '=':
			token = eqsym;
			if(lflag == 1){ printf("	=	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case ',':
			token = commasym;
			if(lflag == 1){ printf("	,	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case '.':
			token = periodsym;
			if(lflag == 1){ printf("	.	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case '<':
			if(nextSymbol == '='){ // check for less than or equal
				token = leqsym;
				if(lflag == 1){ printf("	<=	%d\n", token); }
				tokenString[p] = token;
				p++;
				break;
			}
			else if(nextSymbol == '>'){ // check for not equal
				token = neqsym;
				if(lflag == 1){ printf("	<>	%d\n", token); }
				tokenString[p] = token;
				p++;
				break;
			}
			else{
				token = lessym;
				if(lflag == 1){ printf("	<	%d\n", token); }
				tokenString[p] = token;
				p++;
				break;
			}
			
		case '>':
			if(nextSymbol == '='){ // check for greater than or equal
				token = geqsym;
				if(lflag == 1){ printf("	>=	%d\n", token); }
				tokenString[p] = token;
				p++;
				break;
			}
			else{
				token = gtrsym;
				if(lflag == 1){ printf("	>	%d\n", token); }
				tokenString[p] = token;
				p++;
				break;
			}
			
		case ';':
			token = semicolonsym;
			if(lflag == 1){ printf("	;	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case ':':
			token = becomessym;
			if(lflag == 1){ printf("	:=	%d\n", token); }
			tokenString[p] = token;
			p++;
			break;

		case '%':
			token = modsym;
			if(lflag == 1){ printf("	%c	%d\n", '%', token); }
			tokenString[p] = token;
			p++;
			break;

		default:
			printf("There was an error in the special symbols switch statement\n");
			break;	
	} 
	return 1;
}

// takes in an input file at command line
void lex(FILE * inputFile, int lflag, int aflag, int vflag){

	memset(tokenString, '\0', sizeof(tokenString)); // fill token list array with '\0'
	char *reservedWords[] = {"const", "var", /*"procedure", "call",*/ "if", "then", /*"else",*/ "while", "do", "read", "write", "odd", /*"return",*/ "begin", "end"}; // char array of pointers to char arrays
	char specialSymbols[] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':', '%'};

	if(lflag == 1){
		printf("Lexeme Table: \n");
		printf("	lexeme	token type\n");
	}
    
	int i = 0;
	char nextChar, currentChar;

	while(!feof(inputFile)){ // go through whole input file
		currentChar = fgetc(inputFile);// read in a character from the input file

		if(!iscntrl(currentChar) && !isspace(currentChar)){ // if not a control character; control character are whitespaces, newline
			if(isalpha(currentChar)){ // if the current character is a letter

				int i = 1;
				char letters[cmax+1] = {}; // initialize array with max number of characters
				letters[0] = currentChar; // insert current character (isalpha) into letter array
				nextChar = fgetc(inputFile); // get the next character from input file

				while((!(isspace(nextChar)) && !(iscntrl(nextChar))) && ((isalpha(nextChar))||(isdigit(nextChar))) ){ // while the next character is not a control character and it is a letter or number 
					letters[i] = nextChar; // insert the next character into the letter array
					nextChar = fgetc(inputFile); // get next character

					i++;
					if(i == 12){ // if the identifier is more than 11 characters long
						printf("Error : Identifier names cannot exceed 11 characters\n"); // this is an error
						exit(0);
						while(isdigit(nextChar) || isalpha(nextChar)){// keep reading in characters until one is neither an alpha or digit then continue analyzing tokens
							nextChar = fgetc(inputFile);
						}
						ungetc(nextChar, inputFile); // return character to input file
						break;
					}
				}
				if(i < 12){ // valid identifier or reserved word
					letterToken(letters, reservedWords, lflag); // call function to tokenize identifier
					ungetc(nextChar, inputFile);
				}					
			} // end if isalpha
			else if(isdigit(currentChar)){ // if a number
				
				int i;
				char numbers[6] = {};
				numbers[0] = currentChar; // since the current character is a digit store it in the number array
				nextChar = fgetc(inputFile); // get next character in the input file
				
				if(isdigit(nextChar)){ // if the character is a digit
					for(i=1;i<=6;i++){ // for loop to check if the number exceeds the minimum length
						if(i==6){ // if the length of the number exceeds the max length
							printf("Error: Numbers cannot exceed 5 digits\n"); // this is an error
							exit(0);
							while(isdigit(nextChar) || isalpha(nextChar)){// keep reading in characters until one is neither an alpha or digit then continue analyzing tokens
								nextChar = fgetc(inputFile);
							}
							memset(numbers, '\0', sizeof(numbers)); // in order to not output this number set the array to empty
							break;
						}
						if(isdigit(nextChar)){ // if the next character is a number
							numbers[0] = currentChar;
							numbers[i] = nextChar; // store it in the numbers array
							nextChar = fgetc(inputFile); // get next character
							continue; // next iteration of for loop							
						}
						else if(isalpha(nextChar)){ // if next character is a letter
							printf("Error: Identifiers cannot begin with a digit\n"); // this is an error
							exit(0);
							while(isdigit(nextChar) || isalpha(nextChar)){ // keep reading in characters until one is neither an alpha or digit then continue analyzing tokens
								nextChar = fgetc(inputFile);
							}
							memset(numbers, '\0', sizeof(numbers)); // in order to not output this number or identifier set the array to empty
							break; // break out of for loop
						}
						else{ // else we can break out of for loop and continue analyzing tokens
							break;
						}
					}			
				}
				else if(isalpha(nextChar)){ // if next character is a letter
					printf("Error: Identifiers cannot begin with a digit\n"); // its an error
					exit(0);
					while(isdigit(nextChar) || isalpha(nextChar)){// keep reading in characters until one is neither an alpha or digit then continue analyzing tokens
						nextChar = fgetc(inputFile);
					}
					memset(numbers, '\0', sizeof(numbers)); // in order to not output this number or identifier set the array to empty
				}
				ungetc(nextChar, inputFile);
				numberToken(numbers, lflag);	
			} // end if isdigit
			else{ // else its a special symbol
				for(i=0;i<=sizeof(specialSymbols)+1;i++){
					if((i>sizeof(specialSymbols)) && (!(iscntrl(currentChar))) && (!(isspace(currentChar)))){ // if not a symbol in the special symbols array then its not valid
						if(feof(inputFile)){
							break;
						}
						else{
							printf("Error: Invalid Symbol\n"); // outputs this message at end of input2
							exit(0);
							break;
						}

					}			
					if(specialSymbols[i] == currentChar){ // check if the character is in the special symbols array
						nextChar = fgetc(inputFile); // gets next character; used for becomessym and comments
						if(currentChar == ':' && nextChar == '='){ // a colon is only used as part of := so it checks for the = in the next character
							specialSymbolToken(currentChar, nextChar, lflag); // calls function to print the special symbol
							break;
						}
						if(currentChar == ':' && nextChar != '='){ // if it's not the = character then it's an invalid symbol error
							ungetc(nextChar, inputFile); // returns the character back to be read in the next iteration
							printf("Error: Invalid symbol\n");
							exit(0);
							break;
						}
						if(currentChar == '<' && nextChar == '='){ // check for less than or equal to symbol
							specialSymbolToken(currentChar, nextChar, lflag); // calls function to print the special symbol
							break;
						}
						if(currentChar == '<' && nextChar == '>'){ // check for not equal to symbol
							specialSymbolToken(currentChar, nextChar, lflag); // calls function to print the special symbol
							break;
						}
						if(currentChar == '>' && nextChar == '='){ // check for greater than or equal to symbol
							specialSymbolToken(currentChar, nextChar, lflag); // calls function to print the special symbol
							break;
						}
						if(currentChar == '/' && nextChar == '*'){ // check for comments
							isComment(currentChar, nextChar, inputFile);
							break;									 
						}
						ungetc(nextChar, inputFile); // returns the character back to be read in the next iteration
						specialSymbolToken(currentChar, nextChar, lflag); // calls function to print the special symbol
						break;
					}
				}			
			} // end special symbol
		}
	} // end whilte loop for file reading
	fclose(inputFile);

	outputTokenList(lflag); //output the token list
	parser(tokenString, aflag, vflag); // pass the token list to parser
}