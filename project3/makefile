CLFAGS = -Wall

project3: project3.o mailbox.o
	gcc $(CFLAGS) project3.o mailbox.o -o project3 -lm

project3.o: project3.c mailbox.h
	gcc $(CFLAGS) -c project3.c -o project3.o -lrt -lpthread -lm

mailbox.o: mailbox.c
	gcc $(CFLAGS) -c mailbox.c -o mailbox.o

clean:
	rm -f project3.o mailbox.o project3
