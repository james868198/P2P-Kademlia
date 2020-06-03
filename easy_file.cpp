#include "easy_file.hpp"

File::File(const char* filename){

	file.open(filename, std::fstream::in | std::fstream::out);
	if(!file){
		file.open(filename, std::fstream::out);
		if(!file){
	    	cerr << "Cannot open file " << filename << endl;
	    	this->~File();
		}
	}
}

File::File(const char* filename, int rw){

	file.open(filename, std::fstream::in | std::fstream::out);
	if(!file){
		if(rw){
			file.open(filename, std::fstream::out);
			if(!file){
		    	cerr << "Cannot open file " << filename << endl;
		    	this->~File();
			}
		}else{
			cerr << "File " << filename << " not found" << endl;
	    	// this->~File();
		}
	}
}

File::~File(){
	if(file){
		file.close();
	}
}

int File::length(){
	if(file){
	    file.seekg (0, file.end);
    	int length = file.tellg();
    	file.seekg (0, file.beg);
    	return length;
    }
    return 0;
}

int File::read(char* readbuf, int len){

	if(file){
    	file.read(readbuf, len);
    	return len;
	}
	return 0;
}

int File::write(char* writebuf, int len){

	if(file){
    	file.write(writebuf, len);
    	return len;
	}
	return 0;
}




