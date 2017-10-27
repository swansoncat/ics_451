#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "tcpheader.h"

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>

//This program was created through the guidance of the youtube video "Socket Programming Tutorial in C for Beginners, parts 1 & 2" by Eduonix Learning Solutions and
//various posts on StackOverflow.com
int main(int argc, char *argv[])
{
	//code to get date/time. time_t is an arithmetic type variable that stores the time in seconds. The function localtime takes a time_t variable and 
	//takes the data from there and puts it into a struct tm. tm is a struct that holds a bunch of ints corresponding to minutes, hours, days, etc.
	//strftime is a function that takes the time data from a pointer to a struct tm, formats it, and stores it into a char array.
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);	
	char s[256]; //Note to self: for some reason, this does not work if the array is set to 64. 
	strftime(s, sizeof(s), "%c", tm);

	int server_socket;
	//We are setting up the same kind of server as we did in the client program. AF_INET parameter for internet socket, SOCK_STREAM for TCP protocol, last parameter not used.
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	//This server address struct is set up pretty much exactly the same as in the client. 
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(45000);
	
	//INADDR_ANY is the same thing as localhost. Uncomment below line and comment the next if working off of linux virtual box.
	//Note to self: when trying to telnet into server, 'lo' is not localhost. Trying to connect to 'lo' won't work, You have to use 'localhost'.
	server_address.sin_addr.s_addr = INADDR_ANY;
	//inet_aton("128.171.24.203", server_address.sin_addr);
	

	
	/************************************************/
	/* Current State: Closed
	/* Event: Passive open, set up socket, buffer
	/* New State: Listen
	/************************************************/
	//This is the equivalent of the connect function call on client program.
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	listen(server_socket, 5);

	int client_socket;
	
		
	//new part of assignment 7
	struct sockaddr_in client_address;
	int client_length;

	/************************************************/
	/* Current State: Listen
	/* Event: Received syn, send syn-ack
	/* New State: Syn received
	/************************************************/
	//The second parameter would contain a struct that has address of client connection. The third parameter would contain size of client connection. Both for data purposes.
	//Below has been modified for assignment 7
	client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_length);
	
	int clientAdd = ntohs(client_address.sin_port);
	printf("The client port is: %d\n", clientAdd);
	//unsigned int clientSeq, clientAck;
	char clientSeq[160];

	recv(client_socket, clientSeq, sizeof(clientSeq), 0);
	//recv(client_socket, &clientAck, sizeof(clientAck), 0);
	//recv(client_socket, &clientAck, sizeof(clientAck), 0);
	printf("The client sequence number is: %s\n", clientSeq);
	//printf("The client sequence number is: %u\n", *(&clientSeq-2));
	//printf("The client sequence number is: %u\n", clientAck);
	//printf("The client acknowledgement number is: %u\n", clientAck);
		
	/************************************************/
	/* Current State: Syn received
	/* Event: Received ack
	/* New State: Established
	/************************************************/
	send(client_socket, s, sizeof(s), 0);
	//puts(s);
	
	/************************************************/
	/* Current State: Established
	/* Event: Received fin, send ack
	/* New State: Close wait
	/************************************************/
	close(server_socket);

	/************************************************/
	/* Current State: Closed
	/* Event: Close, send fin, receive ack for fin
	/* New State: Closed
	/************************************************/
	return 0;
}
