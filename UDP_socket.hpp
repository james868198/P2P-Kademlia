#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H


#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/socket.h> 
#include <sys/types.h>		
#include <netdb.h>			/* getaddrinfo */
#include <arpa/inet.h>		// inet_pton()
#include <errno.h>			/* ERRORNO */
#include <ifaddrs.h>
#include <unistd.h>			/* close() */
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <iomanip>			
#include <stdlib.h>			/* system() */

using namespace std;


class Server_socket{
private:	
	int sock = 0;
	char ip[256] = "";
	char port[32] = "";
	struct sockaddr servaddr;

public:
	Server_socket(const char* _port);

	int recv(char* recvbuf, struct sockaddr* cliaddr, int len);
	operator bool() const { return sock > 0; }
	bool valid(){ return sock > 0; };
	const char* get_ip(){ return ip; };
	const char* get_port(){	return port; };
	const int get_sock(){ return sock; };
	const struct sockaddr* get_addr(){ return & servaddr; };
};

class Client_socket{
private:	
	int sock = 0;
	char ip[256] = "";
	char port[32] = "";
	struct sockaddr servaddr;

public:
	Client_socket(const char* _ip, const char* _port);

	int send(const char* sendbuf, int len);
	operator bool() const { return sock > 0; }
	bool valid(){ return sock > 0; };
	const char* get_ip(){ return ip; };
	const char* get_port(){	return port; };
	const int get_sock(){ return sock; };
	const struct sockaddr* get_addr(){ return & servaddr; };
};

bool checkHostName(int hostname);
bool checkHostEntry(struct hostent * hostentry);
bool checkIPbuffer(char *IPbuffer);
string addrstr(struct sockaddr* info);


#endif