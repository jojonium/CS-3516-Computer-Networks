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
	int s, new_fd, r, bytes_read, file;
	socklen_t addr_size;
	struct sockaddr_storage their_addr;
	struct addrinfo hints;
	struct addrinfo *servinfo; // will point to the results
	char *received = (char *)malloc(BUFSIZE * sizeof(char));
	char *reqline[3], path[1024];
	char data_to_send[1024];
	char *root = getenv("PWD"); // root directory of the server

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

	while (1) {
		if (fork()) {
			new_fd = accept(s, (struct sockaddr *)&their_addr, &addr_size);
			
			r = recv(new_fd, received, BUFSIZE, 0);
	
			if (r < 0) {
				printf("recv error\n");
			} else if (r == 0) {
				printf("client disconnected\n");
			} else { // message received
				printf("%s", received);
				reqline[0] = strtok(received, " \t\n");
	
				if (strncmp(reqline[0], "GET\0", 4) == 0) {
					reqline[1] = strtok(NULL, " \t");
					reqline[2] = strtok(NULL, " \t\n");
					if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
						write(new_fd, "HTTP/1.1 400 Bad Request\n", 25);
					} else {
						if (strncmp(reqline[1], "/\0", 2) == 0) {
							reqline[1] = "/index.html"; // send index.html as default if no file is specified
						}
						
						strcpy(path, root);
						strcpy(&path[strlen(root)], reqline[1]);
						printf("file requested: %s\n", path);
	
						if ((file = open(path, O_RDONLY)) != -1) { // file found
							send(new_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
							while ((bytes_read = read(file, data_to_send, 1024)) > 0) {
								write(new_fd, data_to_send, bytes_read);
							}
						} else {
							write(new_fd, "HTTP/1.1 404 Not Found\r\n\r\n", 26); // file not found
						}
					}
				}
			}
	
			close(new_fd);
		}
	}

	return 0;
}
