#ifndef MAILBOX_H_
#define MAILBOX_H_

#define RANGE 1
#define ALLDONE 2

struct msg {
	int iSender; /* sender of the message (0 .. number-of-threads) */
	int type;    /* its type */
	int value1;  /* first value */
	int value2;  /* second value */
};

int SendMsg(int iTo, struct msg *pMsg);
int RecvMsg(int iFrom, struct msg *pMsg);

#endif
