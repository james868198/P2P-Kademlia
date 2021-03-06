#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <iostream>
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

#define BACKLOG 10     // how many pending connections queue will hold


class Server_socket{
	
	int sock;
	char ip[256];
	char port[32];
	
public:
	Server_socket(const char* _port);
	int accept();
	int recv(){
		return 0;
	};

	bool valid(){ return sock > 0; };
	const char* get_ip(){ return ip; };
	const char* get_port(){	return port; };
	const int get_sock(){ return sock; };
	
	// ~Server_socket(){ 
	// 	cout << "socket closed\n";
	// 	close(sock);
	// };
};


bool checkHostName(int hostname);
bool checkHostEntry(struct hostent * hostentry);
bool checkIPbuffer(char *IPbuffer);
string addrstr(struct sockaddr* info);


#endif