CFLAGS = -Wall

all: http_client http_server

http_client: client.c
	gcc $(CFLAGS) -o http_client client.c -lm

http_server: server.c
	gcc $(CFLAGS) -o http_server server.c -lm

clean:
	rm -f http_client http_server
