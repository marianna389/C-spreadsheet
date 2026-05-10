all: program
clean: 
	rm program table.o parse.o calculate.o plugin.o main.o
program: table.o parse.o calculate.o plugin.o main.o
	gcc table.o parse.o calculate.o plugin.o main.o -ldl -o program
main.o: main.c parse.h table.h calculate.h plugin.h
	gcc -c main.c -o main.o
calculate.o: calculate.c table.h calculate.h plugin.h parse.h
	gcc -c calculate.c -o calculate.o
plugin.o: plugin.c plugin.h
	gcc -c plugin.c -o plugin.o
parse.o: parse.c table.h parse.h
	gcc -c parse.c -o parse.o
table.o: table.c table.h
	gcc -c table.c -o table.o
