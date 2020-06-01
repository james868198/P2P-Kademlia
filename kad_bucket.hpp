#ifndef KAD_BUCKET_H
#define KAD_BUCKET_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <list>
#include <algorithm>
#include <sys/stat.h>	//mkdir()
#include <dirent.h>
#include "kad_util.hpp"


class SHA_1;

class Node{
public:
	char 	ip[IP_size] = "";
	char 	port[PORT_size] = "";
	SHA_1 	ID;

	Node(){};
	Node(const char* _ip, const char* _port, const SHA_1 _id);
	bool operator == (const Node& _a) const ;
};

class K_Buck{

	int k = 0;
	list<Node> list;

public:
	K_Buck(){};
	K_Buck(int _k);
	vector<Node> get();
	bool insert(const Node& _node);
};

class DHT{

	SHA_1 ID;
	unordered_map<string, Node> nodes;
	unordered_map<string, string> files;
	vector<K_Buck> buckets;

	static int distance(const SHA_1& _a, const SHA_1& _b);
	int distance(const SHA_1& _key);
	
public:
	DHT(){};
	DHT(const SHA_1& _key);
	void join();
	void insert(const Node& _node);
	vector<Node> get_node(const SHA_1& _key);
	string get_file(const SHA_1& _key);
	void ls_file();

};








#endif