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
void responsePacket(struct tcpheader*, struct tcpheader*, unsigned int, unsigned char, unsigned int);

//Port number can be given as command line argument.
int main(int argc, char *argv[])
{	
	srand(time(NULL));
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	int portNumber = 45000;
	if (argc != 1)
	{
		portNumber = atoi(argv[0]);
	}
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portNumber);
	server_address.sin_addr.s_addr = INADDR_ANY;
	//inet_aton("128.171.24.203", &server_address.sin_addr);
	

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
	
	
	
	
	if (connection_status == -1) 
	{
		printf("There an a error connecting to the server.\n");
	}

	//Begin TCP handshake
	
	write(network_socket, &syn, sizeof(syn));
	sentPacket(&syn);
	if (connection_status == 0)
	{	
		read(network_socket, &response, sizeof(response));
		receivedPacket(&response);
		responsePacket(&response, &ack, syn.seqNo, (unsigned char) 16, (unsigned int) 0);
		write(network_socket, &ack, sizeof(ack));
		sentPacket(&ack);
	}	
	
	
	//End TCP handshake
	
	
	/* This section of the code deals with the transfer of th jpg file. The main loop runs until the server sends it a specific piee of data
	 * indicating that it is done. As of currently it is looking for the string 'end' but this may be changed later. Before the loop starts it
	 * first reads the first jpg binary data from the server. When the loop begins is writes this binary data out to the File, then it reads the
	 * accompanying header to the data, and calls on some functions to print the information in the header it receives and the header it will send
	 * and then it sends its acknowledgement. Lastly it reads the next data from the server which will be the next section of binary data for the jpg.
	 * TLDR: read then write jpg data, read header, print header, create ack header, print ack header, sent ack header, repeat.
	 *
	 * -the sequence number is set to whatever the sequence number of the response set in the handshake was as there was no data sent. There
	 * is no functionality in this loop to change the sequence number as the client never sends data.
	 * -we clear the data in the tcp header structs 'response' and 'ack', the former being used to store the header sent by the server and
	 * the latter being used to store the data to be sent in the ACK response. It isn't cleared after each iteration though so this may not
	 * be necessary, but it was noticed that it was still working even though we didn't have it in the loop.
	 *
	 *
	 */
	FILE *newJPG;
	newJPG = fopen("new.jpg", "w");
	char buffer[1500];
	unsigned int seqNew = response.seqNo;
	bzero(&response, sizeof(response));
	bzero(&ack, sizeof(ack));
	unsigned int readSize = read(network_socket, buffer, 1500);
	while (strcmp(buffer, "end") != 0)
	{
		fwrite(buffer,1,readSize,newJPG);
		read(network_socket, &response, sizeof(response));	
		receivedPacket(&response);
		responsePacket(&response, &ack, seqNew, (unsigned char) 16, readSize);
		write(network_socket, &ack, sizeof(ack));
		sentPacket(&ack);
		readSize = read(network_socket, buffer, 1500);		
	}	
	//End file transfer

	
	//Begin closing handshake	
	bzero(&response, sizeof(response));
	bzero(&ack, sizeof(ack));	
	read(network_socket, &response, sizeof(response));	
	while (response.controlFlags != 1)
	{			
		//This literally does nothing, only exists to loop until it receives an FIN
	}
	receivedPacket(&response);
	bzero(&ack, sizeof(ack));
	responsePacket(&response, &ack, seqNew, (unsigned char) 17, 0);
	write(network_socket, &ack, sizeof(ack));
	sentPacket(&ack);
	bzero(&response, sizeof(response));
	read(network_socket, &response, sizeof(response));
	receivedPacket(&response);	
	//End closing handshake.

	
	fclose(newJPG);
	close(network_socket);
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

void responsePacket(struct tcpheader *original, struct tcpheader *response, unsigned int seq, unsigned char flagz, unsigned int ackMod)
{
	(*response).sourcePort = (*original).destPort;
	(*response).destPort = (*original).sourcePort;
	(*response).seqNo = seq;
	(*response).ackNo = (*original).seqNo + ackMod;
	(*response).dataOffset = (unsigned char) 0;
	(*response).reserved = (unsigned char) 0;
	(*response).controlFlags = flagz;
	(*response).window = (unsigned short) 1500;
	(*response).checksum = (unsigned short) 65535;
	(*response).urgentP = (unsigned short) 0;
}