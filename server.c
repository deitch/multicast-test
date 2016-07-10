#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int fd;
struct sockaddr_in addr;
int addr_size;
int optval = 1;
char *buf;
struct addrinfo *remote_address_info;
struct in_addr localInterface;
struct ip_mreq mreq;

int main(int argc, char* argv[]) {
	char *server_ip = argv[1], *server_port = argv[2], msgbuf[50];
	int nbytes, msglen = sizeof(msgbuf);
	
	// create socket to listen
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket() to receive UDP packets failed");
  } else {
		printf("socket().... OK\n");
	}
	
	// set reuse
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
  } else {
		printf("setsockopt SO_REUSEADDR.... OK\n");
	}
	
  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(atoi(server_port));
	addr_size = sizeof(addr);
 
 	// bind to listen
  if (bind(fd,(struct sockaddr *) &addr,addr_size) < 0) {
		perror("bind failed");
		exit(1);
  } else {
		printf("bind to socket for INADDR_LOOPBACK.... OK\n");
  }

  // join the multicast group
  mreq.imr_multiaddr.s_addr=inet_addr(server_ip);
  mreq.imr_interface.s_addr=htonl(INADDR_ANY);
  if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
 	 perror("setsockopt join group membership");
	 exit(1);
 	} else {
		printf("setsockopt join group membership %s:%s .... OK\n", server_ip, server_port);
	}
	
	// now just enter a loop to listen for packets
  while (1) {
		addr_size=sizeof(addr);
		if ((nbytes=recv(fd,msgbuf,msglen,0)) < 0) {
      perror("recv");
      exit(1);
		}
    printf("message = \"%.*s\"\n", nbytes, msgbuf);
  }
}
