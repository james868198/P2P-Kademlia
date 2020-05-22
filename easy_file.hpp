#ifndef EASY_FILE_H
#define EASY_FILE_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>

using namespace std;

class File{

	fstream file;

public:
	File(const char* filename);

	operator bool() const { return file.is_open(); };
	int length();
	int read(char* readbuf, int len);
	int write(char* writebuf, int len);
	~File();

};

#endif
