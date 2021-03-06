#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "tcpheader.h"

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>

void receivedPacket(struct tcpheader*);
void sentPacket(struct tcpheader*);
void responsePacket(struct tcpheader*, struct tcpheader*, unsigned int, unsigned char);

//This program was created through the guidance of the youtube video "Socket Programming Tutorial in C for Beginners, parts 1 & 2" by Eduonix Learning Solutions and 
//user posts on StackOverflow.
int main(int argc, char *argv[])
{
	srand(time(NULL));
	int network_socket;
	//AF_INET is a parameter meaning internet socket, SOCK_STREAM is a parameter meaning tcp socket, the last parameter indicates the protocol type which is already assumed from last
	//parameter.
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	//variable 'server_address' needs to have same parameter as the 'network_socket'
	server_address.sin_family = AF_INET;
	//The function htons converts the number into the correct format.
	server_address.sin_port = htons(45000);
	
	//INADDR_ANY is the same thing as localhost. Uncomment below line and comment the next if working off of linux virtual box.
	server_address.sin_addr.s_addr = INADDR_ANY;
	//inet_aton("128.171.24.203", &server_address.sin_addr);
	
	/************************************************/
	/* Current State: Closed
	/* Event: Program start, open socket, set up buffer, send syn
	/* New State: Syn sent
	/************************************************/
	//This creates the connection. It returns a int, 0 if success, -1 if not, that can be used to test if connection was successful.
	printf("Beginning client program...\n\n");
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	
	
	struct sockaddr_in client_address;
	int client_size = sizeof(client_address);
	getsockname(network_socket, (struct sockaddr *) &client_address, &client_size);
	struct tcpheader syn, response, ack;
	syn.sourcePort = (unsigned short) ntohs(client_address.sin_port);
	syn.destPort = (unsigned short) 45000;
	syn.seqNo = (unsigned int) rand() % 10001;
	syn.ackNo = (unsigned int) 0;
	syn.dataOffset = (unsigned char) 0;//has to be combined with reserved using a bitwise shift or something
	syn.reserved = (unsigned char) 0;//has to be combined with data offset using a bitwise shift or something
	syn.controlFlags = (unsigned char) 2;
	syn.window = (unsigned short) 65535;
	syn.checksum = (unsigned short) 65535;
	syn.urgentP = (unsigned short) 0;
	
	
	/*Used this while trying to figure out how to get client port. Don't want to delete in case need to look at again.
	 *
	int clientAdd = ntohs(client_address.sin_port);
	printf("The client port is: %u\n", syn.sourcePort);
	printf("The sequence is: %u\n", syn.seqNo);
	*/
	
	
	if (connection_status == -1) 
	{
		printf("There an a error connecting to the server.\n");
	}


	
	send(network_socket, &syn, sizeof(syn), 0);
	sentPacket(&syn);
	if (connection_status == 0)
	{	
		recv(network_socket, &response, sizeof(response), 0);
		receivedPacket(&response);
		responsePacket(&response, &ack, syn.seqNo, (unsigned char) 8);
		sentPacket(&ack);
	}	
	
	/************************************************/
	/* Current State: Syn sent
	/* Event: Received syn ack, sent ack
	/* New State: Established
	/************************************************/
	/* Response from earlier server program. Don't want to delete in case need to look at again.
	 *
	char server_response[256];
	if (connection_status == 0)
	{	
		recv(network_socket, &server_response, sizeof(server_response), 0);

	}
	printf("The date and time is: %s\n", server_response);
	*/
	
	/************************************************/
	/* Current State: Established
	/* Event: close, send fin
	/* New State: Closed
	/************************************************/
	
	
	
	close(network_socket);
	
	

	/************************************************/
	/* Current State: Closed
	/* Event: receive ack for fin, receive fin, send ack
	/* New State: Closed
	/************************************************/
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