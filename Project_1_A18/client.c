#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#define BUFSIZE 8192

int main(int argc, char *argv[]) {
	char *server_url;
	char *filename; 
	char *port_number;
	char *msg = (char *)malloc(BUFSIZE * sizeof(char));
	char *received = NULL;
	char *block = (char *)malloc(BUFSIZE * sizeof(char));
	int g, s, c, r, len, bytes_sent;
	long rtt;
	int count = 1;
	int printFlag = 0;
	int serverRead = 0;
	int portRead = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo; // will point to the results
	struct timeval start, end;

	if (argc < 3) {
		printf("Not enough arguments. Expected the form:\n\t ./http_client [-options] server_url port_number\n");
		exit(1);
	}
	
	// parse command line arguments
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') { // option
			if (argv[i][1] == 'p')
				printFlag = 1;
		} else {
			if (!serverRead) {
				server_url = argv[i];
				serverRead = 1;
			} else if (!portRead) {
				port_number = argv[i];
				portRead = 1;
			}
		}
	}

	// split server_url into the url (e.g. "example.com") and file (e.g. "/index.html")
	char *temp = strstr(server_url, "/");
	if (temp) {
		filename = (char *)malloc(BUFSIZE * sizeof(char));
		strcpy(filename, temp);
		*temp = '\0';
	} else {
		filename = "/index.html";
	}

	printf("printFlag: %d\n", printFlag);
	printf("server_url: %s\n", server_url);
	printf("filename: %s\n", filename);
	printf("port_number: %s\n", port_number);

	strcat(msg, "GET ");
	strcat(msg, filename);
	strcat(msg, " HTTP/1.1\r\nHost: ");
	strcat(msg, server_url);
	strcat(msg, "\r\n\r\n");
	len = strlen(msg);

	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets


	// get ready to connect
	if ((g = getaddrinfo(server_url, port_number, &hints, &servinfo)) < 0) { // error
		printf("getaddrinfo error\n");
		exit(1);
	}

	if ((s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0) { //error
		printf("socket error\n");
		exit(1);
	}

	// record start time
	gettimeofday(&start, NULL);

	if ((c = connect(s, servinfo->ai_addr, servinfo->ai_addrlen)) < 0) { // error
		printf("connect error\n");
		exit(1);
	}

	// record end time
	gettimeofday(&end, NULL);

	// SEND IT
	if ((bytes_sent = send(s, msg, len, 0)) < 0) { // error
		printf("send error\n");
		exit(1);
	}

	if (bytes_sent < len) { // not all data sent
		printf("not all data sent\n");
		exit(1);
	}

	// Receive it
	printf("Ready to receive\n");
	while (1) {
		if ((r = recv(s, block, BUFSIZE, 0)) < 0) {
			printf("recv error\n");
		} else if (r == 0) {
			printf("nothing received\n");
			break;
		} else {
			printf("%s", block);
			//received = (char *)realloc(received, count * BUFSIZE * sizeof(char));
			//strcat(received, block);
			//printf("Reallocated. Count = %d\n", count);
			//count++;
		}

	}

	// calculate RTT
	rtt = ((end.tv_sec * 1000) + (end.tv_usec / 1000)) - ((start.tv_sec * 1000) + (start.tv_usec / 1000));
	if (printFlag) {
		printf("\n=== RTT: %ld milliseconds ===\n\n", rtt);
	}

	printf("\nDONE\n");
	//printf("final count: %d\n", count);
	//printf("=== RECEIVED ===\n\n%s\n", received);
	return 0;
}
