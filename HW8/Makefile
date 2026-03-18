FILES = add.c div.c mul.c sub.c

all : calc clean

calc : libcalc.so
	gcc main.c -o calc -L. -lcalc

libcalc.so : objects
	gcc -shared *.o -o libcalc.so

objects : 
	gcc $(FILES) -fPIC -c

clean :
	rm *.o

run :
	LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./calc