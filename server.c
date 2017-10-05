#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>


//This program was created through the guidance of the youtube video "Socket Programming Tutorial in C for Beginners, parts 1 & 2" by Eduonix Learning Solutions and
//various posts on StackOverflow.com
int main(int argc, char *argv[])
{
	char server_message[256] = "you have reached the server";
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	//For some reason, this does not work if the array is set to 64.
	char s[256];
	strftime(s, sizeof(s), "%c", tm);

	int server_socket;
	//We are setting up the same kind of server as we did in the client program.
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	//This server address struct is set up pretty much exactly the same as in the client.
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(45000);
	//server_address.sin_addr.s_addr = INADDR_ANY;

	inet_aton("128.171.24.203", server_address.sin_addr);
	
	//
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	listen(server_socket, 5);

	int client_socket;

	//The second parameter would contain a struct that has address of client connection. The third parameter would contain size of client connection.
	client_socket = accept(server_socket, NULL, NULL);

	send(client_socket, s, sizeof(s), 0);
	close(server_socket);

	return 0;
}
