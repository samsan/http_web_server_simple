#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/*	
============================================================================ 

	A simple HTTP server.

	The goal: understand how things work.
	
	Thanks to JP Bruins Slot's great tutorial
	https://bruinsslot.jp/post/simple-http-webserver-in-c/#fn1

============================================================================
*/

#define PORT 8080
#define BUFFER_SIZE 2048

int main(void){

	// Create the buffer for READ operations
	char buffer[BUFFER_SIZE];

	// Server reponse 200
	char resp[] = "HTTP/1.0 200 OK\r\n"
        	        "Server: webserver-c\r\n"
                	"Content-type: text/html\r\n\r\n"
                  	"<html>hello, world</html>\r\n";

	// Create a socket
	int tcp_socket_fd = socket(AF_INET,SOCK_STREAM,0); 

	// Check for errors
	if (tcp_socket_fd == -1){
		perror("Webserver (socket creation)");
		return 1;
	}
	printf("Socket created successfully\n");

	// Create the address to bind the socket to
	struct sockaddr_in socket_address;
	int socket_address_length = sizeof(socket_address);

	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(PORT);
	// listen on 0.0.0.0
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// Create a client address
	struct sockaddr_in client_address;
	int client_address_len = sizeof(client_address);

	// -------------------------------------------------------- BIND
	// Bind the socket to the adress
	if(bind(tcp_socket_fd,(struct sockaddr *)&socket_address,socket_address_length) != 0){
		perror("Webserver (socket bind)");
		return 1;
	}

	// -------------------------------------------------------- LISTEN
	// Use the socket as a passive one
	if(listen(tcp_socket_fd, SOMAXCONN) != 0){
		perror("Webserver (listen)");
		return 1;
	}

	// -------------------------------------------------------- ACCEPT
	while(true){
		int new_socket_fd = accept(tcp_socket_fd,(struct sockaddr *)&socket_address,(socklen_t *)&socket_address_length);
		if(new_socket_fd < 0){
			perror("Webserver (accept)");
			return 1;
		}
		printf("connection accepted\n");
		// Get client IP and port
		int socket_name = getsockname(new_socket_fd, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len);
		if(socket_name == -1){
			perror("Webserver (getsockname)");
			continue;
		}

		printf("%s:%d\n",inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

		// ------------------------------------------------ READ
		int values_read = read(new_socket_fd, buffer, BUFFER_SIZE);
		if(values_read == -1 ){
			perror("Webserver (read)");
			continue;
		}

		char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
		sscanf(buffer, "%s %s %s", method, uri, version);
		printf("%s %s %s\n", method, uri, version);

		// ------------------------------------------------ WRITE
		if(write(new_socket_fd,resp,strlen(resp)) == -1){
			perror("Webserver (write)");
			continue;
		}

		close(new_socket_fd);
	}

	return 0;
}
