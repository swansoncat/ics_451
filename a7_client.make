a7_server: a7_server.o
	cc a7_server.o -o a7_server

a7_client: a7_client.o
	cc a7_client.o -o a7_client

a7_client.o: a7_client.c
	cc -c a7_client.c

a7_server.o: a7_server.c
	cc -c a7_server.c
