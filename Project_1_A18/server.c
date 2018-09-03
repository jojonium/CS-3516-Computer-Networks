#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>


#define BACKLOG 10
#define BUFSIZE 8096

int main(int argc, char *argv[]) {
	int s, new_fd, r, bytes_sent, file_to_send, remaining;
	socklen_t addr_size;
	struct sockaddr_storage their_addr;
	struct addrinfo hints;
	struct addrinfo *servinfo; // will point to the results
	char *received = (char *)malloc(BUFSIZE * sizeof(char));
	char *message;
	struct stat file_stat;
	long offset, len;

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

	message = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
	len = strlen(message);
	if ((file_to_send = open("TMDG.html", O_RDONLY)) < 0) {
		printf("open error\n");
		exit(1);
	}

	// get file stats
	if (fstat(file_to_send, &file_stat) < 0) {
		printf("fstat error\n");
		exit(1);
	}
	
	while (1) {
		new_fd = accept(s, (struct sockaddr *)&their_addr, &addr_size);
		/*if ((r = recv(new_fd, received, BUFSIZE, 0)) < 0) {
			printf("recv error\n");
			exit(1);
		}*/
		
		recv(new_fd, received, BUFSIZE, 0);

		// add length of sent file to len
		len += file_stat.st_size;

		// send header
		if ((s = send(new_fd, message, len, 0)) < 0) {
			printf("sending header error");
			exit(1);
		}

		offset = 0;
		remaining = file_stat.st_size;
		
		// send file data
		while (((bytes_sent = sendfile(new_fd, file_to_send, &offset, BUFSIZ)) > 0) && (remaining > 0)) {
			remaining -= bytes_sent;
		}

		close(new_fd);
	}



	return 0;
	
}
