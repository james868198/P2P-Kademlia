// #ifndef KAD_UTIL_H
// #define KAD_UTIL_H

#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <fstream>
#include "UDP_socket.hpp"
#include "easy_file.hpp"
#include <openssl/sha.h>
#include <sys/stat.h>	//mkdir()
#include <vector>
#include <memory>		//unique_ptr
using namespace std;


#define IP_size 32
#define PORT_size 32
#define File_size 256
#define Head_size 0

typedef void * (*THREADFUNCPTR)(void *);


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
	char* get(){ return key; };
	bool operator == (const SHA_1& a);
	bool operator != (const SHA_1& a){ return !(*this == a); };
	bool operator == (const unsigned char* a);
	bool operator != (const unsigned char* a){ return !(*this == a); };
	static unsigned char* to_hash(const char* _key);
};

class RPC{
private:
	time_t tx_time;
	time_t rx_time;
public:
	char ip[IP_size];
	char port[PORT_size];
	SHA_1 srcID;
	SHA_1 dstID;
	char msg[32] = "";
	char ack;
	shared_ptr<char> data;
	RPC(){};
	RPC(const SHA_1 _id, const char* _msg, const char _ack);

	void request();
	void response();

};

class RPC_Manager{
private:
	vector<shared_ptr<RPC>> RPC_list;
	shared_ptr<RPC> parse(const char* _buf, const int _len);
public:
	RPC_Manager(){};

	void handle(const char* _buf, const int _len);
	static void* RPCThread(void *);
};

void help();
void print_time(char* result);
bool get_config(const char* filename);
void* serverThread(void* p);

// variables

extern bool RUNNING;

extern char config_file[256];
extern char bootstrap[32];
extern char local_port[32];
extern char local_ip[32];
extern SHA_1 local_id;
extern int local_k;
extern int local_alpha;
extern char shared_folder[256];
extern char download_folder[256];

// #endif