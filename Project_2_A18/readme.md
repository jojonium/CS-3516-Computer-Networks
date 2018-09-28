## Project2

Project 2 is a sending and receiving transport-level code for implementing a
simple reliable data transfer protocol, i.e. the Alternating-Bit-Protocol (ABP).
`project2.c` simulates the underlying hardware/software environment, while
`student2.c` has the ABP code (written by me). The following picture shows how
ABP is implemented in this assignment:

![ABP Diagram](https://raw.githubusercontent.com/jojonium/CS3516-Computer-Networks/master/Project_2_A18/ABP-diagram.png)

To compile the project on GNU/Linux, simply use `make`. Then, run it with
`./p2`. The program will prompt for several variables, including:

* The number of messages to simulate
* The probability that each packet will be lost by the network
* The probability that each packet will be corrupted
* The probability that each packet will arrive out of order
* The average amount of time between messages
* The tracing level: A value of 1 or 2 will print out useful information about
  what is going on inside the emulation. A tracing value of 0 will turn this
  off. A tracing value of 5 will display all sorts of odd messages that are for
  emulator-debugging purposes.
* Randomization: 0 means the same events will happen each time the program is
  run, 1 means events will be randomized.
* Bidirectional: set this to 0, bidirection sending is not supported.

Each of these variables can also be supplied on the command line, for example:

`% ./p2 1000 .5 .5 .7 10 1 1 0`

`test.txt` shows a the result of running the program with 1000 packets, a 90%
chance of packet loss, a 90% chance of packet corruption, a 90% chance of
packets being out of order, 10 ms of time between messages from sender's layer
5, level 1 tracing, and actions randomized.
