#include<stdio.h>
#include<stdlib.h>
#include "tcpheader.h"

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>

//This program was created through the guidance of the youtube video "Socket Programming Tutorial in C for Beginners, parts 1 & 2" by Eduonix Learning Solutions and 
//user posts on StackOverflow.
int main(int argc, char *argv[])
{
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
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	
	
	struct sockaddr_in client_address;
	int client_size = sizeof(client_address);
	getsockname(network_socket, (struct sockaddr *) &client_address, &client_size);
	struct tcpheader syn;
	syn.sourcePort = (unsigned short) ntohs(client_address.sin_port);
	syn.destPort = (unsigned short) 45000;
	syn.seqNo = (unsigned int) rand() % 10001;
	syn.ackNo = (unsigned int) 7777;
	syn.dataOffset = (unsigned char) 0;//has to be combined with reserved using a bitwise shift or somethin
	syn.reserved = (unsigned char) 0;//has to be combined with data offset using a bitwise shift or something
	syn.controlFlags = (unsigned char) 2;
	syn.window = (unsigned short) 65535;
	syn.checksum = (unsigned short) 65535;
	syn.urgentP = (unsigned short) 9999;
	
	
	int clientAdd = ntohs(client_address.sin_port);
	printf("The client port is: %u\n", syn.sourcePort);
	printf("The sequence is: %u\n", syn.seqNo);
	
	if (connection_status == -1) 
	{
		printf("There an a error connecting to the server.\n");
	}

	
	//This huge block puts the data from the TCP header into a hexadecimal string.
	char data[160];	
	int stringOffset = sizeof(syn.sourcePort);
	sprintf(data, "%x" , syn.sourcePort);
	sprintf(data+stringOffset, "%x" , syn.destPort);
	stringOffset = stringOffset + sizeof(syn.destPort);	
	sprintf(data+stringOffset, "%x" , syn.seqNo);	
	stringOffset = stringOffset + sizeof(syn.seqNo);	
	sprintf(data+stringOffset, "%x" , syn.ackNo);
	stringOffset = stringOffset + sizeof(syn.ackNo);
	sprintf(data+stringOffset, "%x" , syn.dataOffset);//this would be a combined dataOffset and reserved, 
	stringOffset = stringOffset + sizeof(syn.dataOffset);
	sprintf(data+stringOffset, "%x" , syn.controlFlags);
	stringOffset = stringOffset + sizeof(syn.controlFlags);
	sprintf(data+stringOffset, "%x" , syn.window);
	stringOffset = stringOffset + sizeof(syn.window);
	sprintf(data+stringOffset, "%x" , syn.checksum);
	stringOffset = stringOffset + sizeof(syn.checksum);
	sprintf(data+stringOffset, "%x" , syn.urgentP);
	
	send(network_socket, data, sizeof(data), 0);

	
	/************************************************/
	/* Current State: Syn sent
	/* Event: Received syn ack, sent ack
	/* New State: Established
	/************************************************/
	char server_response[256];
	if (connection_status == 0)
	{	
		recv(network_socket, &server_response, sizeof(server_response), 0);

	}
	
	
	
	printf("The date and time is: %s\n", server_response);
	
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
