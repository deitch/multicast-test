#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int fdsend, fdsendto;
struct in_addr interface_addr;
int addr_size;
int optval = 1;
char *bufsend = "send()", *bufsendto = "sendto()";
struct addrinfo *remote_address_info;
struct in_addr localInterface;

int main(int argc, char* argv[]) {
	char *remote_ip = argv[1], *local_ip = "127.0.0.1", *remote_port = argv[2];
	// create first and second socket
	if ((fdsend = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket() send() failed");
	}
	if ((fdsendto = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
  	perror("socket() sendto() failed");
	}
	
	// set socket options on both
	if (setsockopt(fdsend, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) < 0) {
		perror("setsockopt(SO_REUSEADDR) send() failed");
	}
	if (setsockopt(fdsendto, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) < 0) {
		perror("setsockopt(SO_REUSEADDR) sendto() failed");
	}
	
	addr_size = sizeof(interface_addr);
	
	// structure the addr
	if (getaddrinfo(remote_ip, remote_port, NULL, &remote_address_info) != 0) {
		perror("getaddrinfo(remote_ip) failed");
	}
	
 
 	// connect on send() sockets ONLY
	if (connect(fdsend, remote_address_info->ai_addr, remote_address_info->ai_addrlen) != 0) {
		perror("connect() send() failed");
	}
	
	// send  and sendto
	if (send(fdsend, bufsend, strlen(bufsend), 0) == -1) {
		perror("send() failed");
	}
	if (sendto(fdsendto, bufsendto, strlen(bufsendto), 0, remote_address_info->ai_addr, remote_address_info->ai_addrlen) == -1) {
		perror("sendto() failed");
	}
}
