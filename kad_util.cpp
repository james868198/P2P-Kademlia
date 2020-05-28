#include "kad_util.hpp"

bool RUNNING = true;

// ==========================================================================================
// configurations
// ==========================================================================================
char config_file[File_size] = "default.config";
char bootstrap[IP_size] = "";
char local_port[PORT_size] = "";
char local_ip[IP_size] = "";
SHA_1 local_id;
int local_k = 0;
int local_alpha = 0;
char shared_folder[File_size] = "Shared/";
char download_folder[File_size] = "Download/";

// ==========================================================================================
RPC_Manager rpc_mng;


// ==========================================================================================
// RPC::
// ==========================================================================================
RPC::RPC(const SHA_1 _id, const char* _msg, const char _ack){
	srcID = _id;
	strcpy(msg, _msg);
	ack = _ack;
}

void RPC::request(){
	// PING 
	char packet[512] = "";
	sprintf(packet, "%s:%s:", local_ip, local_port);
	sprintf(packet+strlen(packet), "%s", local_id.get());
    sprintf(packet+strlen(packet), "|PING|%c|", ack);
    sprintf(packet+strlen(packet), "%s", srcID.get());
    sprintf(packet+strlen(packet), "|");
    // get node ip and port from the routing tree
    Client_socket node(bootstrap, local_port);
    if(node){
	    node.send(packet, strlen(packet));
	    printf("<< %s\n", packet);
    }
}

void RPC::response(){
	ack = '1';
	char packet[512] = "";
	sprintf(packet, "%s:%s:", local_ip, local_port);
	sprintf(packet+strlen(packet), "%s", local_id.get());
    sprintf(packet+strlen(packet), "|PING|%c|", ack);
    sprintf(packet+strlen(packet), "%s|", srcID.get());
    // get node ip and port from the routing tree
    Client_socket node(ip, port);
    if(node){
	    node.send(packet, strlen(packet));
	    printf("<< %s\n", packet);
    }
}

// ==========================================================================================
// RPC_Manager::
// ==========================================================================================
void RPC_Manager::handle(const char* _buf, const int _len){

	// shared_ptr<RPC> rpc = shared_ptr<RPC>(parse(_buf, _len), [](RPC* p){std::cout << "[deleter called]\n"; delete p;});
	RPC* rpc = resolve(_buf, _len);
	if(!rpc){
		printf("rpc discarded.\n");
		return;
	}
	// printf("%s\n%s\n", local_id.get(), rpc->dstID.get());
	if(local_id != rpc->dstID){
		// discard
		printf("rpc discarded.\n");
	}else{
		printf("rpc received.\n");
		if(rpc->ack == '0'){
			RPC_list.push_back(rpc);
			printf("list size : %d\n", (int)RPC_list.size());

			pthread_t thread_ID = 0;
			pthread_create(&thread_ID, NULL, (THREADFUNCPTR)RPCThread, (void*)rpc);
		}else{

		}
	}
}

RPC* RPC_Manager::resolve(const char* _buf, const int _len){

	// shared_ptr<RPC> ret(new RPC, [](RPC* p){std::cout << "[deleter called]\n"; delete p;});
	// shared_ptr<RPC> ret = make_shared<RPC>();
	RPC* ret = new RPC;
	// ret->data = shared_ptr<char>(new char (_len));
	
	char srcKey[64] = "";
	char dstKey[64] = "";

	char* pos = (char*)_buf;
	// char* tok = 0;
	int n = 0;
	int len = 0;
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

	len = _len+_buf-pos;
	if(pos <= _buf+_len){
		ret->data = new char [len];
		memcpy(ret->data, pos, len);
		// strcpy(ret->data, pos);
	}
	if(ret->ack == '0'){
		pos = ret->data;
		if(!strcmp(ret->msg, "STORE")){
			if(!(n = strstr(pos, "|") - pos)) 	return NULL;
			char key[64] = "";
			strncpy(key, pos, n); 		pos += n+1;
			ret->key = SHA_1(SHA_1::to_hash(key));
			if(!(n = strstr(pos, "|") - pos)) 	return NULL;
			strncpy(ret->name, pos, n); 	pos += n+1;
			if(pos > ret->data+len) 			return NULL;
			char num[64] = "";
			strncpy(num, pos, len+ret->data-pos);
			ret->len = atoi(num);

		}else if(!strcmp(ret->msg, "FIND_NODE")){
			if(!(n = strstr(pos, "|") - pos)) 	return NULL;
			char nodeid[64] = "";
			strncpy(nodeid, pos, n); 		pos += n+1;
			ret->ID = SHA_1(SHA_1::to_hash(nodeid));
		}else if(!strcmp(ret->msg, "FIND_VALUE")){
			if(!(n = strstr(pos, "|") - pos)) 	return NULL;
			char key[64] = "";
			strncpy(key, pos, n); 		pos += n+1;
			ret->key = SHA_1(SHA_1::to_hash(key));
		}else{
			printf("else\n");
		}
	}

	// printf("%s\n", ret->ip);
	// printf("%s\n", ret->port);
	// printf("%s\n", ret->srcID.get());
	// printf("%s\n", ret->msg);
	// printf("%c\n", ret->ack);
	// printf("%s\n", ret->dstID.get());
	// printf("%s\n", ret->data);
	return ret;
}

void* RPCThread(void * p){

	// RPC_Manager RPC_mng = *(RPC_Manager*)p;
	// shared_ptr<RPC> rpc((RPC*)p);
	RPC* rpc = (RPC*)p;
	// needs response
	if(!strcmp(rpc->msg, "PING")){
		rpc->response();
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
	// delete [] rpc->data;
	// delete rpc;
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

// void* RPC_Manager::RPCThread(void * p){
// 	struct RPC_struct rpc_strc = *(struct RPC_struct*)p;
// 	// RPC_Manager RPC_mng = *(RPC_Manager*)p;
// 	// shared_ptr<RPC> rpc((RPC*)p);
// 	// RPC* rpc = (RPC*)p;
// 	// shared_ptr<RPC> rpc = RPC_mng.RPC_list.back();
// 	shared_ptr<RPC> rpc = *(shared_ptr<RPC>*)rpc_strc.rpc;
// 	RPC_Manager RPC_mng = *(RPC_Manager*)rpc_strc.RPC_mng;
// 	if(rpc->ack == '0'){
// 		// needs response
// 		if(!strcmp(rpc->msg, "PING")){
// 			rpc->response();
// 		}else{
// 			printf("else\n");
// 		}
// 	}else{
// 		printf("response\n");
// 	}
// 	for(auto it=RPC_mng.RPC_list.begin(); it!=RPC_mng.RPC_list.end(); ++it){
// 		if(*it == rpc){
// 			RPC_mng.RPC_list.erase(it);
// 		}
// 	}
// 	return nullptr;
// }

// ==========================================================================================
// server
// ==========================================================================================
void* serverThread(void* p){

	Server_socket server(local_port);
	if(!server){
		cerr << "Server initialization failed.\n";
		RUNNING = false;
		return 0;
	}
	strcpy(local_ip, server.get_ip());
	printf("local ip  : %s\n", local_ip);

	char str[64] = "";
	sprintf(str, "%s:%s", local_ip, local_port);
	local_id = SHA_1(str);

	printf("Node ID   : %s\n", local_id.get());

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

bool SHA_1::operator == (const SHA_1& a){
	for(int i=0; i<SHA_DIGEST_LENGTH; ++i){
		if(a.hash[i] != this->hash[i]){
			return false;
		}
	}
	return true;
}

bool SHA_1::operator == (const unsigned char* a){
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
		char str[256] = "";
		string attr = "";
		char val[256] = "";
		
		while(config >> attr >> val){
			if(attr == "bootstrap"){
				strcpy(bootstrap, val);
			}else if(attr == "port"){
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
    	int status = mkdir(shared_folder, 0777);
    	// if(status!=0)
    	mkdir(download_folder, 0777);

		printf("-----------------------------------\n");
		printf("    Configurations      \n");
		printf("-----------------------------------\n");
		printf("bootstrap : %s\n", bootstrap);
		printf("port      : %s\n", local_port);
		printf("k         : %d\n", local_k);

	}
	return true;
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
