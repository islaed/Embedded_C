FILES = add.c div.c mul.c sub.c

all: calc clean

calc : libcalc.a
	gcc main.c -L. -lcalc -o calc

libcalc.a : objects
	ar rcs libcalc.a *.o

objects :
	gcc -c $(FILES)

clean: 
	rm *.o libcalc.a