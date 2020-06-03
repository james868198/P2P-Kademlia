#ifndef KAD_UTIL_H
#define KAD_UTIL_H

#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <fstream>
#include "UDP_socket.hpp"
#include "easy_file.hpp"
#include <openssl/sha.h>

#include <vector>
#include <memory>		//unique_ptr

using namespace std;


#define IP_size 32
#define PORT_size 32
#define File_size 256
#define Head_size 0

#define t_Threshold 10

typedef void * (*THREADFUNCPTR)(void *);

class K_Buck;
class DHT;


class SHA_1{
private:
	char str[256] = "";
	unsigned char hash[SHA_DIGEST_LENGTH] = "";
	char key[64] = "";
public:
	SHA_1(){};
	SHA_1(const char* _str);
	SHA_1(const unsigned char* _hash);

	void set(const unsigned char* _hash);
	const char* get() const { return key; };
	unsigned char* get_hash() const { return (unsigned char*)hash; };

	bool operator == (const SHA_1& a) const;
	bool operator != (const SHA_1& a) const { return !(*this == a); };
	bool operator == (const unsigned char* a) const;
	bool operator != (const unsigned char* a) const { return !(*this == a); };


	static unsigned char* to_hash(const char* _key);
};

class RPC{
private:
	time_t tx_time;
	time_t rx_time;

	pthread_t thread_ID = 0;
	
	void* ret;
public:
	bool 	block = false;

	char 	ip[IP_size] = "";
	char 	port[PORT_size] = "";
	SHA_1 	srcID;
	// ---------------------------
	char 	msg[32] = "";
	char 	ack;
	SHA_1 	dstID;
	// ---------------------------
	char* 	data;
	SHA_1 	key;	// key, value
	char 	name[File_size] = "";	// filename
	int 	len = 0;	// file length
	SHA_1 	ID;	// node id

	RPC* 	response = 0;

	RPC(){};
	RPC(const SHA_1& _id, const char* _msg, const char _ack, bool _block);
	RPC(const char* _ipp, const char* _msg, const char _ack, bool _block);

	void* request();
	void* respond();
	void print() const;
	void* get_response(){ return ret; };
	static bool match(const RPC* _a, const RPC* _b);
	static void* requestThread(void * p);
	static void* respondThread(void * p);
};

class RPC_Manager{
private:
	vector<RPC*> RPC_list;
	RPC* resolve(const char* _buf, const int _len);
public:
	RPC_Manager(){};

	void handle(const char* _buf, const int _len);
	void push(RPC* _rpc);
	void remove(RPC* _rpc);
};

void help();
void print_time(char* result);
bool get_config(const char* filename);
void* serverThread(void* p);
void* RPCThread(void * p);
const char* stripp(const char* _ip, const char* _port);

// variables

extern bool RUNNING;

extern char config_file[File_size];
extern char boot_ip[IP_size];
extern char boot_port[PORT_size];
extern char local_port[PORT_size];
extern char local_ip[IP_size];
extern SHA_1 local_id;
extern int local_k;
extern int local_alpha;
extern char shared_folder[File_size];
extern char download_folder[File_size];

// extern RPC_Manager rpc_mng;
extern RPC_Manager rpc_mng;
extern DHT dht;
extern Server_socket server;

#endif