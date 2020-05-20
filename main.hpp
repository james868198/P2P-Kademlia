#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include "TCP_socket.hpp"

using namespace std;

void help(){
	
	cout << 
	"main - CEON 317 P2P-Kademlia\n\n" <<
		
	"USAGE:\n" <<
	"      ./main -h\n" <<
	"      ./main -c <config_file> \n\n" <<

	"OPTIONS:\n" <<
	"      -h, -help                  get this help page\n" <<
	"      -c, -config <config_file>  configure file directory (default is ./default.config)\n" <<

	"EXAMPLE:\n" <<
	"      ./main -c /file/setup1.config\n" << endl;
	
}

void print_time(char* result){
	
	time_t rawtime;
	time(&rawtime);
	const struct tm * timeptr = gmtime(&rawtime);
  	static const char wday_name[][4] = {
    	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  	};
  	static const char mon_name[][4] = {
   	 	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
   	 	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  	};
  	//Wed, 22 Jul 2009 19:15:56 GMT
  	sprintf(result, "%.3s,%3d %.3s %d %.2d:%.2d:%.2d GMT",
    	wday_name[timeptr->tm_wday], 
		timeptr->tm_mday, mon_name[timeptr->tm_mon],
    	1900 + timeptr->tm_year,
		timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);
    
}


#endif