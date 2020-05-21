#include "UDP_socket.hpp"


bool checkHostName(int hostname){ 
    if(hostname == -1){ 
        perror("gethostname"); 
        return false;
    } 
	return true;
} 
  
// Returns host information corresponding to host name 
bool checkHostEntry(struct hostent * hostentry){ 
    if(hostentry == NULL){ 
        perror("gethostbyname"); 
        return false;
    } 
	return true;
} 
  
// Converts space-delimited IPv4 addresses 
// to dotted-decimal format 
bool checkIPbuffer(char *IPbuffer){ 
    if(NULL == IPbuffer){ 
        perror("inet_ntoa"); 
        return false;
    }
	return true;
}

int Server_socket::recv(char* recvbuf, struct sockaddr* cliaddr, int len){
	socklen_t addlen;

	int n = recvfrom(sock, recvbuf, len, MSG_WAITALL, cliaddr, &addlen);
	recvbuf[n] = '\0'; 
	
    return n;
}

Server_socket::Server_socket(const char* _port){
		
	// To retrieve local host ip 
	int hostname = gethostname(ip, sizeof(ip)); 
	if(!checkHostName(hostname)){
		return ;
	}
  
	// To retrieve local host information 
	struct hostent* host_entry = gethostbyname(ip); 
	if(!checkHostEntry(host_entry)){
		return ;
	}
	// To convert an Internet network 
	// address into ASCII string 
	strcpy(ip, inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]))); 
	strcpy(port, _port);

	struct addrinfo *servinfo;
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints);
	sock = 0;
	/* IPv4  AF_INET  don't care AF_UNSPEC */
	hints.ai_family = AF_INET; 
	/* TCP stream sockets = SOCK_STREAM;*/
	// UDP socket
	hints.ai_socktype = SOCK_DGRAM;

	if((status = getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
	    cout << "getaddrinfo error: " << gai_strerror(status) << endl;
		cout << "exit." << endl;
	    return ;
	}

	if((sock = ::socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		/* failed to create socket */
		perror("socket() failed");
		sock = 0;
		return ;
	}
	int on = 1;
	if (setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0){
	    perror("setsockopt() failed");
		close(sock);
		sock = 0;
		return ;
	}

	if(ioctl(sock, FIONBIO, (char *)&on) < 0){
		perror("ioctl() failed");
		close(sock);
		sock = 0;
		return ;
	}

	if(::bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		/* failed to bind */
		perror("socket() failed");
		close(sock);
		sock = 0;
		return ;
	}
	servaddr = *servinfo->ai_addr;
	freeaddrinfo(servinfo);
}

int Client_socket::send(const char* sendbuf, int len){
	int n = sendto(sock, (const char *)sendbuf, len, 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(struct sockaddr)); 
	return n;
}

Client_socket::Client_socket(const char* _ip, const char* _port){
		
	struct addrinfo *servinfo;
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints);
	sock = 0;
	/* IPv4  AF_INET  don't care AF_UNSPEC */
	hints.ai_family = AF_INET; 
	// UDP socket
	hints.ai_socktype = SOCK_DGRAM;

	if((status = getaddrinfo(_ip, _port, &hints, &servinfo)) != 0) {
	    cout << "getaddrinfo error: " << gai_strerror(status) << endl;
		cout << "exit." << endl;
	    return ;
	}

	if((sock = ::socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		/* failed to create socket */
		perror("socket() failed");
		sock = 0;
		return ;
	}
	servaddr = *servinfo->ai_addr;
	freeaddrinfo(servinfo);
}

string addrstr(struct sockaddr* info){
	
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)info;
	void *addr = &(ipv4->sin_addr);
	char ipstr[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));		
	uint16_t port = ntohs(ipv4->sin_port);

	return string(ipstr) + ":" + to_string(port);
}
