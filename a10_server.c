#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include "tcpheader.h"

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>

void receivedPacket(struct tcpheader*);
void sentPacket(struct tcpheader*);
void responsePacket(struct tcpheader*, struct tcpheader*, unsigned int, unsigned char);

/* This server program is designed to take a filename as the first command line argument and a port number as a second.
 *
 */
int main(int argc, char *argv[])
{
	
	/* Code to get date/time.
	 *
	 * -time_t is an arithmetic type variable that stores the time in seconds.
	 * -The function localtime() takes a time_t pointer variable and takes the data from there and puts it into a struct tm.
	 * -tm is a struct that holds a bunch of ints corresponding to minutes, hours, days, etc.
	 * -strftime is a function that takes the time data from a pointer to a struct tm, formats it, and stores it into a char array.
	 * -srand() is a function that seeds the random number generator used by the function rand().
	 */
	time_t t = time(NULL);
	srand(time(NULL));
	struct tm *tm = localtime(&t);	
	char s[256]; //Note to self: for some reason, this does not work if the array is set to 64. 
	strftime(s, sizeof(s), "%c", tm);

	
	/* Code to set up socket that server will listen for connections from.
	 *
	 * -For socket function, first parameter is for address family (AF_INET for IPv4, AF_INET6 for IPv6), second parameter is for communication type
	 * (SOCK_STREAM for TCP, SOCK_DGRAM for UDP), and the third parameter is the protocol number, in which '0' is the default for TCP.
	 *
	 * -sockaddr_in is a struct that holds 3 things: address family (like first parameter of socket function), port number, and IP address.
	 *
	 * -The socket() function creates the socket, creating the sockaddr_in struct and assigning its variables creates the socket infromation,
	 *  and the bind function combines the two to create the working socket.
	 */
	int server_socket, client_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);	
	if (server_socket < 0)
	{
		perror("Error opening server socket.");
	}
	
	struct sockaddr_in server_address, client_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(45000);	
	server_address.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY binds socket to all available interfaces,
	//inet_aton("128.171.24.203", server_address.sin_addr);
	int bindTest = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));	
	if (bindTest < 0)
	{
		perror("Error binding server socket.");
	}
	
	
	listen(server_socket, 5);
	printf("Waiting for connections...\n\n");
	
		
	int client_length;
	client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_length);
	if (client_socket < 0)
	{
		perror("Error on accepting connection on server socket.");
	}
	
	struct tcpheader received, toBeSent, recvAck;
	struct tcpheader *a = &received;
	struct tcpheader *b = &recvAck;

	read(client_socket, a, sizeof(received));	
	receivedPacket(a);
	responsePacket(a, &toBeSent, rand() % 10001, (unsigned char) 10);
	sentPacket(&toBeSent);

		
	write(client_socket, &toBeSent, sizeof(toBeSent));
	read(client_socket, b, sizeof(recvAck));
	receivedPacket(b);

	
	close(server_socket);
	close(client_socket);
	return 0;
}

void receivedPacket(struct tcpheader *packet)
{
	printf("Received a tcp packet with these header fields.\n");
	printf("Source port: %u\n", (*packet).sourcePort);
	printf("Destination port: %u\n", (*packet).destPort);
	printf("Sequence number: %u\n", (*packet).seqNo);
	printf("Acknowledgement number: %u\n", (*packet).ackNo);
	printf("Data offset: %u\n", (*packet).dataOffset);
	printf("Reserved: %u\n", (*packet).reserved);
	printf("Control flags: %u\n", (*packet).controlFlags);
	printf("Window: %u\n", (*packet).window);
	printf("Checksum: %u\n", (*packet).checksum);
	printf("Urgent pointer: %u\n\n", (*packet).urgentP);
}

void sentPacket(struct tcpheader *packet)
{
	printf("Sent a tcp packet with these header fields.\n");
	printf("Source port: %u\n", (*packet).sourcePort);
	printf("Destination port: %u\n", (*packet).destPort);
	printf("Sequence number: %u\n", (*packet).seqNo);
	printf("Acknowledgement number: %u\n", (*packet).ackNo);
	printf("Data offset: %u\n", (*packet).dataOffset);
	printf("Reserved: %u\n", (*packet).reserved);
	printf("Control flags: %u\n", (*packet).controlFlags);
	printf("Window: %u\n", (*packet).window);
	printf("Checksum: %u\n", (*packet).checksum);
	printf("Urgent pointer: %u\n\n", (*packet).urgentP);
}

void responsePacket(struct tcpheader *original, struct tcpheader *response, unsigned int seq, unsigned char flagz)
{
	(*response).sourcePort = (*original).destPort;
	(*response).destPort = (*original).sourcePort;
	(*response).seqNo = seq;
	(*response).ackNo = (*original).seqNo;
	(*response).dataOffset = (unsigned char) 0;
	(*response).reserved = (unsigned char) 0;
	(*response).controlFlags = flagz;
	(*response).window = (unsigned short) 65535;
	(*response).checksum = (unsigned short) 65535;
	(*response).urgentP = (unsigned short) 0;
}