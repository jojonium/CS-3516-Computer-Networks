#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
 
/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/

// linked list node struct for the queue
struct node {
	struct pkt *packet;
	struct node *next;
};

// GLOBAL VARIABLES
int A_seq, B_seq;
struct pkt lastSent;
struct node *head = NULL;
struct node *tail = NULL;

/********************************************\
               HELPER FUNCTIONS
\********************************************/
int calculateCS(struct pkt packet) {
	int out, i;
	
	out = (13 * packet.seqnum) + (17 * packet.acknum);
	for (i = 0; i < MESSAGE_LENGTH; i++) {
		out += (i + 1) * packet.payload[i];
	}
	
	return out;
}

struct pkt *pop() {
	struct pkt *packet = head->packet;
	struct node *oldHead = head;
	head = head->next;

	free(oldHead);

	if (head == NULL) tail = NULL;

	return packet;
}

void push(struct pkt *packet) {
	struct node *newNode = malloc(sizeof(struct node));
	newNode->packet = packet;
	newNode->next = NULL;

	// if queue is empty
	if (head == NULL && tail == NULL) {
		head = newNode;
		tail = newNode;
	}
	else {
		tail->next = newNode;
		tail = newNode;
	}
}

void sendFromQueue() {
	struct pkt *packet;

	packet = pop();

	if (TraceLevel >= 2) {
		printf("A sending a packet:\t");
		printf("payload: %.20s\tsequence: %d\n", packet->payload, packet->seqnum);
	}

	tolayer3(AEntity, *packet);
	startTimer(AEntity, 500);
	lastSent = *packet;

	free(packet);
}

// returns 1 for a corrupt packet, 1 for corruption
int checkCorrupt(struct pkt packet) {
	int checksum = calculateCS(packet);
	
	if (TraceLevel >= 2) {
		printf("Corruption check: calculated %d, expected %d\n", checksum, packet.checksum);
	}

	return !(checksum == packet.checksum);
}

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */

/* 
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
	struct pkt packet;

	// flip sequence number and set it
	A_seq = !A_seq;
	packet.seqnum = A_seq;

	packet.acknum = 0;

	// put in the payload
	strncpy(packet.payload, message.data, MESSAGE_LENGTH);

	// set the checksum
	packet.checksum = calculateCS(packet);

	if (TraceLevel >= 2) {
		printf("A sending a packet:\t");
		printf("payload: %.20s\tsequence: %d\n", packet.payload, packet.seqnum);
	}

	lastSent = packet;
	tolayer3(AEntity, packet);
}

/*
 * Just like A_output, but residing on the B side.  USED only when the 
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {
}

/* 
 * A_input(packet), where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the B-side (i.e., as a result 
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side. 
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
	if (TraceLevel >= 2) {
		printf("A received a packet:\t");
		printf("acknum: %d\tseqnum: %d\n", packet.acknum, packet.seqnum);
	}

	// check for corruption
	if (checkCorrupt(packet)) {
		if (TraceLevel >=2) {
			printf("Response corrupted. Resending last packet\n");
		}
		tolayer3(AEntity, lastSent);
		startTimer(AEntity, 500);
	}

	// NAK
	else if (packet.acknum == FALSE) {
		if (TraceLevel >= 2) {
			printf("A Received NAK... ");
		}

		// see if B wants the last packet resent
		if (packet.seqnum == lastSent.seqnum) {
			if (TraceLevel >= 2) {
				printf("B asked for the last packet to be retransmitted.\n");
			}
			tolayer3(AEntity, lastSent);
			startTimer(AEntity, 500);
		}

		// see if B wants the next packet
		else {
			if (TraceLevel >= 2) {
				printf("B asked for the next packet.\n");
			}

			if (head != NULL)
				sendFromQueue();
			
		}
	}

	// ACK
	else {
		if (head != NULL)
			sendFromQueue();
		// if the head is NULL it means the queue is empty so we do nothing
	}
}


/*
 * A_timerinterrupt()  This routine will be called when A's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
	// timed out, resend last packet
	if (TraceLevel >= 2) {
		printf("Timed out, resending last packet\n");
	}
	tolayer3(AEntity, lastSent);
	startTimer(AEntity, 500);
} 

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
	lastSent.seqnum = 1;
	lastSent.acknum = 0;
	for (int i = 0; i < MESSAGE_LENGTH; i++) {
		lastSent.payload[i] = 'A';
	}
	lastSent.checksum = 0;
	A_seq = 1;
}


/* 
 * Note that with simplex transfer from A-to-B, there is no routine  B_output() 
 */

/*
 * B_input(packet),where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the A-side (i.e., as a result 
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side. 
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
	struct pkt response;
	struct msg message;

	// check for corruption
	if (checkCorrupt(packet) || packet.seqnum != B_seq) {
		if (TraceLevel >= 2) {
			printf("B received a bad packet: ");
			if (packet.seqnum != B_seq)
				printf("the sequence number is wrong.\n");
			else
				printf("it's corrupt");
		}

		// make NAK
		response.seqnum = B_seq;
		response.acknum = FALSE;
		response.checksum = calculateCS(response);
	}
	
	// good packet
	else {
		if (TraceLevel >= 2) {
			printf("B received a packet:\t");
			printf("payload: %.20s\tsequence: %d\n", packet.payload, packet.seqnum);
		}

		// flip B_seq
		B_seq = !B_seq;

		// extract the message and pass it to layer 5
		strncpy(message.data, packet.payload, MESSAGE_LENGTH);
		if (TraceLevel >= 2) {
			printf("Sending message to layer 5: %.20s\n", message.data);
		}
		tolayer5(BEntity, message);

		// make ACK
		response.seqnum = packet.seqnum;
		response.acknum = TRUE;
		response.checksum = calculateCS(response);
	}

	tolayer3(BEntity, response);
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
}

/* 
 * The following routine will be called once (only) before any other   
 * entity B routines are called. You can use it to do any initialization 
 */
void B_init() {
	B_seq = 0;
}

