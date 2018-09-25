CFLAGS = -Wall -g

all: p2

p2: project2.o student2.o
	gcc $(CFLAGS) project2.o student2.o -o p2

project2.o: project2.c
	gcc $(CFLAGS) -c project2.c -o project2.o

student2.o: student2.c project2.h
	gcc $(CFLAGS) -c student2.c -o student2.o

clean:
	rm -f student2.o project2.o p2
