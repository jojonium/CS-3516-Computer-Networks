CS 3516 Project 1 -- Joseph Petitti

Simple HTTP Server and Client in C

To compile the project, simply type the command 'make'. This will compile the two source files,
client.c and server.c, into executable files http_client and http_server respectively. To run the
server, use the command './http_server port_num', where port_num is the port you want the server
to listen on. To run the client, use the command './htp_client [-p] server_url port_num', where
server_url is the address you want to connect to and port_num is the port you want to connect on.
Include the -p switch to print out the RTT for accessing the URL.
