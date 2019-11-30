all: lab3
	
lab3.o: lab/lab3.c
	gcc -c -o lab3.o lab3/lab3.c

lab3: lab3.o
	gcc lab3/lab3.c -o lab3
