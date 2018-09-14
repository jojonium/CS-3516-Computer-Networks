CS 3516 - Computer Networks
===========================

Coursework and assignments for the WPI computer science class CS 3516 - Computer Networks.

## Lab 1

The purpose of lab 1 is to get students familiar with the Wireshark packet capture tool. I used this
program to capture and analyze internet packets from my home network. With Wireshark running I
visited a website in a browser, then looked at the types of packets that were sent to and from my
computer. "lab1-get.pdf" and "lab1-ok.pdf" show the two main HTML packets involved, a GET request
from my computer and a 200 OK response from the server. The folder also includes a "pcapng" file that
shows the results of Wireshark during the lab.

## Lab 2

Lab 2 is designed to give students an understanding of DNS servers, DNS queries, and responses. The
screenshots and pcapng files show details of Wireshark captures of DNS queries and responses, and the
writeup (lab2.pdf) goes into more detail about these messages.

## Project 1

The goal of project 1 is to implement a HTTP client and server using a simplified version of the
HTTP/1.1 protocol in C using the Unix socket commands.

To compile the project, simply type the command `make`. This will compile the two source files,
`client.c` and `server.c`, into executable files `http_client` and `http_server` respectively. To 
run the server, use the command `./http_server port_num`, where `port_num` is the port you want the 
server to listen on. To run the client, use the command `./htp_client [-p] server_url port_num`, where
`server_url` is the address you want to connect to and `port_num` is the port you want to connect on.
Include the `-p` switch to print out the RTT for accessing the URL.

### Client

The HTTP client is run with the command `./http_client [-options] server_url port_number`. It
constructs and sends a valid HTTP/1.1 GET request to the specified `server_url` on the specified
`port_number` (the default HTTP port is 80). It reads the server's response into a dynamically
allocated buffer, then prints it onto the terminal. If the `-p` switch is included, it also measures
the round trip time to connect to the server and prints that on the terminal as well.

### Server

The HTTP server is run with the command `./http_server port_number`. The server starts listening on
the given port number in an infinite loop (it must be interrupted with a termination signal to stop
it). When a client connects on the given port number, the server reads the HTTP request and extracts
the requested filename from it. If no file is requested the default is "index.html".  If the file
exists in the server's root directory is returns an HTTP 200 OK response to the client, followed by
the file. A sample "index.html" file has been included for testing purposes. If the requested file
doesn't exist the server sends a 404 Not Found response to the client.

After it's done sending the file the server closes the client connection and loops back to wait for
another client to connect. When the server is terminated it shuts down gracefully, closing any open
sockets and freeing allocated memory.

The server is also multi-threaded, meaning it can handle multiple clients at the same time by forking
off child processes every time a client connects.
