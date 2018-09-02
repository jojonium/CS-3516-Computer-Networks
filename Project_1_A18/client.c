#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#define BUFSIZE 8096

int main(int argc, char *argv[]) {
	char *server_url;
	char *port_number;
	char *msg = (char *)malloc(BUFSIZE * sizeof(char));
	char *received = (char *)malloc(BUFSIZE * sizeof(char));
	int g, s, c, r, len, bytes_sent;
	long r_size = BUFSIZE;
	int printFlag = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo; // will point to the results

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
			if (server_url == NULL) {
				server_url = argv[i];
			} else {
				port_number = argv[i];
				break;
			}
		}
	}

	printf("printFlag: %d\n", printFlag);
	printf("server_url: %s\n", server_url);
	printf("port_number: %s\n", port_number);

	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	// get ready to connect
	if ((g = getaddrinfo(server_url, port_number, &hints, &servinfo)) < 0) { // error
		printf("getaddrinfo error");
		exit(1);
	}

	// servinfo now points to a linked list of 1 or more struct addrinfos

	// we should do error checking on getaddrinfo(), and walk the linked list
	// looking for valid entries instead of just assuming the first one is good
	s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if (s < 0) { // error
		printf("socket error\n");
		exit(1);
	}

	if ((c = connect(s, servinfo->ai_addr, servinfo->ai_addrlen)) < 0) { // error
		printf("connect error\n");
		exit(1);
	}

	strcat(msg, "GET /index.html HTTP/1.1\r\nHost: ");
	strcat(msg, server_url);
	strcat(msg, "\r\n\r\n");

	len = strlen(msg);
	if ((bytes_sent = send(s, msg, len, 0)) < 0) { // error
		printf("send error\n");
		exit(1);
	}

	if (bytes_sent < len) { // not all data sent
		printf("not all data sent\n");
		exit(1);
	}

	if((r = recv(s, received, r_size, 0)) < 0) { // error
		printf("recv error");
	}

	printf("received: %s", received);
	return 0;
}
