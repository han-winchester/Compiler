
# Created by Han Winchester
# gcc -o output lex.o parser.o vm.o   This line creates an output file called "output" then you can run it as ./output <filename>.txt in terminal
# without -o output it creates a.out file
# compiles object code files
output: main.o lex.o parser.o vm.o
	gcc main.o lex.o parser.o vm.o 

# dependent on main.c and the header files
main.o: main.c compiler.h
	gcc -c main.c

# dependent on lex.c then compiles the c files 
lex.o: lex.c compiler.h
	gcc -c lex.c

# is dependent on the c and header files; compiles the c files 
parser.o: parser.c compiler.h
	gcc -c parser.c

vm.o: vm.c compiler.h
	gcc -c vm.c

# run in terminal as "make clean" then it removes all object and output files
clean:
	rm *.o *.out