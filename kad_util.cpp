#include "kad_util.hpp"
#include "kad_bucket.hpp"

bool RUNNING = true;

// ==========================================================================================
// configurations
// ==========================================================================================
char 	config_file[File_size] 		= "default.config";
char 	boot_ip[IP_size] 			= "";
char 	boot_port[PORT_size] 		= "";
char 	local_ip[IP_size] 			= "";
char 	local_port[PORT_size] 		= "";
SHA_1 	local_id;
int 	local_k 					= 0;
int 	local_alpha 				= 0;
char 	shared_folder[File_size] 	= "Shared/";
char 	download_folder[File_size] 	= "Download/";

// ==========================================================================================

// ==========================================================================================
// RPC::
// ==========================================================================================
RPC::RPC(const SHA_1& _id, const char* _msg, const char _ack){
	dstID = _id;
	strcpy(msg, _msg);
	ack = _ack;
}

RPC::RPC(const char* _ipp, const char* _msg, const char _ack){
	
	dstID = SHA_1(_ipp);
	strcpy(msg, _msg);
	ack = _ack;
}

void RPC::request(){
	// PING 
	char packet[512] = "";
	sprintf(packet, "%s:%s:", local_ip, local_port);
	sprintf(packet+strlen(packet), "%s", local_id.get());
    sprintf(packet+strlen(packet), "|PING|%c|", ack);
    sprintf(packet+strlen(packet), "%s", dstID.get());
    sprintf(packet+strlen(packet), "|");
    // get node ip and port from the routing tree
    vector<Node> list = dht.get_node(dstID);
    if(list.size()){
    	Client_socket node(list.back().ip, list.back().port);
	    if(node){
		    node.send(packet, strlen(packet));
		    printf("<< %s\n", packet);
	    }
    }
    
}

void RPC::respond(){
	pthread_create(&thread_ID, NULL, (THREADFUNCPTR)RPCThread, (void*)this);
}

void RPC::print(){
	printf("ip    :%s\n", ip);
	printf("port. :%s\n", port);
	printf("srcID :%s\n", srcID.get());
	printf("msg   :%s\n", msg);
	printf("ack   :%c\n", ack);
	printf("dstID :%s\n", dstID.get());
	printf("data  :%s\n", data);
}

bool RPC::match(RPC& _rpc){
	if(srcID != _rpc.dstID){
		return false;
	}
	if(dstID != _rpc.srcID){
		return false;
	}
	if(strcmp(msg, _rpc.msg)){
		return false;
	}
	return true;
}

// ==========================================================================================
// RPC_Manager::
// ==========================================================================================
void RPC_Manager::handle(const char* _buf, const int _len){

	RPC* rpc = resolve(_buf, _len);
	if(!rpc){
		printf("rpc discarded.\n");
		return;
	}
	if(local_id != rpc->dstID){
		// discard
		printf("rpc discarded.\n");
	}else{
		printf("rpc received.\n");
		if(rpc->ack == '0'){
			RPC_list.push_back(rpc);
			printf("list size : %d\n", (int)RPC_list.size());
			rpc->respond();
		}else{

		}
	}
}

RPC* RPC_Manager::resolve(const char* _buf, const int _len){

	RPC* ret = new RPC;
	char srcKey[64] = "";
	char dstKey[64] = "";

	char* pos = (char*)_buf;
	// char* tok = 0;
	int n = 0;
	
	if(!(n = strstr(pos, ":") - pos)) return NULL;
	strncpy(ret->ip, pos, n); 	pos += n+1;
	
	if(!(n = strstr(pos, ":") - pos)) return NULL;
	strncpy(ret->port, pos, n); 	pos += n+1;
	
	if(!(n = strstr(pos, "|") - pos)) return NULL;
	strncpy(srcKey, pos, n); 		pos += n+1;
	ret->srcID = SHA_1(SHA_1::to_hash(srcKey));
	
	if(!(n = strstr(pos, "|") - pos)) return NULL;
	strncpy(ret->msg, pos, n); 	pos += n+1;
	
	if(!(n = strstr(pos, "|") - pos)) return NULL;
	strncpy(&ret->ack, pos, n); 	pos += n+1;
	
	if(!(n = strstr(pos, "|") - pos)) return NULL;
	strncpy(dstKey, pos, n); 		pos += n+1;
	ret->dstID = SHA_1(SHA_1::to_hash(dstKey));


	if(ret->ack == '0'){
		// pos = ret->data;
		if(!strcmp(ret->msg, "PING")){
			;
		}else if(!strcmp(ret->msg, "STORE")){
			if(!(n = strstr(pos, "|") - pos)){
				delete ret;
				return NULL;
			} 	
			char key[64] = "";
			strncpy(key, pos, n); 		pos += n+1;
			ret->key = SHA_1(SHA_1::to_hash(key));
			if(!(n = strstr(pos, "|") - pos)){
				delete ret;
				return NULL;
			} 	
			strncpy(ret->name, pos, n); 	pos += n+1;
			if(pos > _buf+_len){
				delete ret;
				return NULL;
			} 	
			char num[64] = "";
			strncpy(num, pos, _buf+_len-pos);
			ret->len = atoi(num);
		}else if(!strcmp(ret->msg, "FIND_NODE")){
			if(!(n = strstr(pos, "|") - pos)){
				delete ret;
				return NULL;
			} 	
			char nodeid[64] = "";
			strncpy(nodeid, pos, n); 		pos += n+1;
			ret->ID = SHA_1(SHA_1::to_hash(nodeid));
		}else if(!strcmp(ret->msg, "FIND_VALUE")){
			if(!(n = strstr(pos, "|") - pos)){
				delete ret;
				return NULL;
			} 	
			char key[64] = "";
			strncpy(key, pos, n); 		pos += n+1;
			ret->key = SHA_1(SHA_1::to_hash(key));
		}else{
			delete ret;
			return NULL;

			printf("else\n");
		}
	}else if(ret->ack == '1'){
		if(!strcmp(ret->msg, "PING")){
			;
		}else if(!strcmp(ret->msg, "STORE")){
			if(!(n = strstr(pos, "|") - pos)){
				delete ret;
				return NULL;
			} 	
			char key[64] = "";
			strncpy(key, pos, n); 		pos += n+1;
			ret->key = SHA_1(SHA_1::to_hash(key));
		}else if(!strcmp(ret->msg, "FIND_NODE")){
			if(!(n = strstr(pos, "|") - pos)){
				delete ret;
				return NULL;
			} 	
			char nodeid[64] = "";
			strncpy(nodeid, pos, n); 		pos += n+1;
			ret->ID = SHA_1(SHA_1::to_hash(nodeid));
		}else if(!strcmp(ret->msg, "FIND_VALUE")){
			if(!(n = strstr(pos, "|") - pos)){
				delete ret;
				return NULL;
			} 	
			char key[64] = "";
			strncpy(key, pos, n); 		pos += n+1;
			ret->key = SHA_1(SHA_1::to_hash(key));
		}else{
			delete ret;
			return NULL;
		}
		int len = _len+_buf-pos;
		if(len > 0){
			ret->data = new char [len];
			memcpy(ret->data, pos, len);
			// strcpy(ret->data, pos);
			printf("new [%d]\n", len);
		}
	}
	// ret->print();
	return ret;
}

void* RPCThread(void * p){

	RPC* rpc = (RPC*)p;
	// needs response
	if(!strcmp(rpc->msg, "PING")){
		rpc->ack = '1';
		char packet[512] = "";
		sprintf(packet, "%s:%s:", local_ip, local_port);
		sprintf(packet+strlen(packet), "%s", local_id.get());
	    sprintf(packet+strlen(packet), "|PING|%c|", rpc->ack);
	    sprintf(packet+strlen(packet), "%s|", rpc->srcID.get());
	    // get node ip and port from the routing tree
	    Client_socket node(rpc->ip, rpc->port);
	    if(node){
		    node.send(packet, strlen(packet));
		    printf("<< %s\n", packet);
	    }
	}else if(!strcmp(rpc->msg, "STORE")){
		// wait for following msg
		while(rpc->ack == '0')
			usleep(1000);

	}else if(!strcmp(rpc->msg, "FIND_NODE")){

	}else if(!strcmp(rpc->msg, "FIND_VALUE")){

	}else{
		printf("else\n");
	}
	rpc_mng.remove(rpc);
	return nullptr;
}

void RPC_Manager::remove(RPC*& rpc){
	for(auto it=RPC_list.begin(); it!=RPC_list.end(); ++it){
		if(*it == rpc){
			RPC_list.erase(it);
			break;
		}
	}
	delete [] rpc->data;
	delete rpc;
}

// ==========================================================================================
// server
// ==========================================================================================
void* serverThread(void* p){

	char recvbuf[1400] = {};
	int n = 0;
	struct sockaddr cliaddr; 
	
	cout << "Waiting..." << endl;
	while(RUNNING){
		if((n = server.recv(recvbuf, &cliaddr, sizeof(recvbuf))) > 0){
			// do something
    		printf(">> %s\n", recvbuf); 
    		rpc_mng.handle(recvbuf, n);

		}else{
			usleep(500);
		}
		
	}
	return 0;
}

// ==========================================================================================
// SHA_1::
// ==========================================================================================
unsigned char* SHA_1::to_hash(const char* _key){
	static unsigned char _hash[SHA_DIGEST_LENGTH];
	for(int i=0; i<SHA_DIGEST_LENGTH; i++){
        sscanf(_key+i*2, "%02x", (unsigned int*)&_hash[i]);
        // printf("%02x", _hash[i]);
    }
	// printf("\n");
	return _hash;
}

SHA_1::SHA_1(const char* _str){
	strcpy(str, _str);
	SHA1((unsigned char*)str, strlen(str), (unsigned char*)hash);
	for(int i=0; i<SHA_DIGEST_LENGTH; i++){
        sprintf(key+2*i, "%02x", hash[i]);
    }
}

SHA_1::SHA_1(const unsigned char* _hash){
	for(int i=0; i<SHA_DIGEST_LENGTH; i++){
		hash[i] = _hash[i];
        sprintf(key+2*i, "%02x", hash[i]);
    }
}

void SHA_1::set(const unsigned char* _hash){
	for(int i=0; i<SHA_DIGEST_LENGTH; i++){
		hash[i] = _hash[i];
        sprintf(key+2*i, "%02x", hash[i]);
    }
}

bool SHA_1::operator == (const SHA_1& a) const{
	for(int i=0; i<SHA_DIGEST_LENGTH; ++i){
		if(a.hash[i] != this->hash[i]){
			return false;
		}
	}
	return true;
}

bool SHA_1::operator == (const unsigned char* a) const{
	for(int i=0; i<SHA_DIGEST_LENGTH; ++i){
		if(a[i] != this->hash[i]){
			return false;
		}
	}
	return true;
}

// ==========================================================================================
// uitl::
// ==========================================================================================
bool get_config(const char* filename){
	ifstream config;
	config.open(filename);
	if(!config){
	    cerr << "Cannot open config file.\n";
	    return false;
	}else{

		string attr = "";
		char val[256] = "";
		
		while(config >> attr >> val){
			if(attr == "bootstrap"){
				strcpy(boot_ip, val);
			}else if(attr == "port"){
				strcpy(boot_port, val);
			}else if(attr == "local_port"){
				strcpy(local_port, val);
			}else if(attr == "k"){
				local_k = atoi(val);
			}else if(attr == "alpha"){
				local_alpha = atoi(val);
			}else if(attr == "shared_folder"){
				strcpy(shared_folder, val);
			}else if(attr == "download_folder"){
				strcpy(download_folder, val);
			}else{
				;
			}
		}

		config.close();
    	
		printf("-----------------------------------\n");
		printf("    Configurations      \n");
		printf("-----------------------------------\n");
		printf("bootstrap : %s\n", boot_ip);
		printf("port      : %s\n", boot_port);
		printf("k         : %d\n", local_k);

		server = Server_socket(local_port);
		if(!server){
			cerr << "Server initialization failed.\n";
			RUNNING = false;
			return 0;
		}
		strcpy(local_ip, server.get_ip());
		printf("local ip  : %s\n", local_ip);
		printf("local port: %s\n", local_port);
		
		local_id = SHA_1(stripp(local_ip, local_port));

		printf("Node ID   : %s\n", local_id.get());

		// deploy the DHT and config the node
		dht = DHT(local_id);

	}
	return true;
}

const char* stripp(const char* _ip, const char* _port){
	static char str[64] = "";
	memset(str, 0, sizeof(str));
	sprintf(str, "%s:%s", _ip, _port);
	return str;
}

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
