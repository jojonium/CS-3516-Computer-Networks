#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

#define BACKLOG 10
#define BUFSIZE 8096

int main(int argc, char *argv[]) {
	int s, new_fd, r;
	socklen_t addr_size;
	struct sockaddr_storage their_addr;
	struct addrinfo hints;
	struct addrinfo *servinfo; // will point to the results
	char *received = (char *)malloc(BUFSIZE * sizeof(char));

	// error check for arguments
	if (argc < 2) {
		printf("Not enough arguments. Expected the form:\n\t ./http_server port_number\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care if IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if (getaddrinfo(NULL, argv[1], &hints, &servinfo) != 0) {
		printf("getaddrinfo error\n");
		exit(1);
	}

	// servinfo now points to a linked list of 1 or more struct addrinfos
	
	if ((s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0) {
		printf("socket error\n");
		exit(1);
	}

	if (bind(s, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		printf("bind error\n");
		exit(1);
	}

	if (listen(s, BACKLOG) < 0) {
		printf("listen error\n");
		exit(1);
	}

	addr_size = sizeof their_addr;

	char *message = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n<!DOCTYPE html><h1>Testing</h1>\r\n\r\n";
	int len = strlen(message);
	int bytes_sent;
	
	while (1) {
		new_fd = accept(s, (struct sockaddr *)&their_addr, &addr_size);
		if ((r = recv(new_fd, received, BUFSIZE, 0)) < 0) {
			printf("recv error\n");
			exit(1);
		}

		bytes_sent = send(new_fd, message, len, 0);
		printf("%d bytes sent\n", bytes_sent);

		close(new_fd);
	}



	return 0;
	
}
