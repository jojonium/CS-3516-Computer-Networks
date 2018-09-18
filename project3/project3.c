#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "mailbox.h"

#define MAXTHREAD 10

// create global variables
struct msg *postOffice;
sem_t *semArray;

int main(int argc, char *argv[]) {
	int inputThreads, i;

	// check arguments
	if (argc < 2) {
		printf("too few arguments\n");
		// TODO print usage
		exit(1);
	}

	inputThreads = atoi(argv[1]);

	if (inputThreads > MAXTHREAD) {
		printf("Input number of threads too high, defaulting to %d\n", MAXTHREAD);
		inputThreads = MAXTHREAD;
	}

	postOffice = (struct msg *)malloc((inputThreads + 1) * sizeof(struct msg));
	semArray = (sem_t *)malloc((inputThreads + 1) * sizeof(sem_t));

	for (i = 0; i <= inputThreads; i++) {
		sem_init((semArray + i), 0, 1);
	}

	return 0;

}
