#ifndef KAD_UTIL_H
#define KAD_UTIL_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include "UDP_socket.hpp"
#include "easy_file.hpp"
#include <openssl/sha.h>
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

class SHA_1{

	char str[256] = "";
	unsigned char hash[SHA_DIGEST_LENGTH] = "";
	char key[64] = "";
public:
	SHA_1(){};
	SHA_1(char* _str);
	char* get(){ return key; };
	bool operator == (const SHA_1& a);
};

class RPC{
	time_t tx_time;
	time_t rx_time;

	SHA_1 id;
	char msg[20] = "";
	char ack;

public:
	RPC(const SHA_1 _id, const char* _msg, const char _ack);

	void request();
	void response();

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

#endif