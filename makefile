_p_1=3
_p_2=2
_p_3=4
_p_4=5

all: lab3
	
lab3.o: lab3/lab3.c
	gcc -c -o lab3.o lab3/lab3.c

lab3: lab3.o
	gcc lab3/lab3.c -o lab3

lab6: lab6/lab6

start_lab6: lab6/lab6
	./lab6/lab6 $(__p_1) $(__p_2)
