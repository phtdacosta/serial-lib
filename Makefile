CC=gcc
CFLAGS=-I.

build:
	gcc -c seriallib.c -o seriallib.o
	ar rcs libseriallib.a seriallib.o
	gcc -c main.c -o main.o
	gcc -c seriallib.c -o seriallib.o
	gcc -shared -o libseriallib.so seriallib.o
	gcc -o main.exe main.c libseriallib.so
