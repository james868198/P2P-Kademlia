#ifndef KAD_UTIL_H
#define KAD_UTIL_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include "TCP_socket.hpp"

using namespace std;

extern bool RUNNING;

extern char config_file[200];
extern char bootstrap[32];
extern char local_port[32];
extern char local_ip[32];
extern int local_k;


void help();
void print_time(char* result);
bool get_config(const char* filename);
void* serverThread(void* p);


#endif