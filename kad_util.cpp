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
RPC::RPC(const SHA_1& _id, const char* _msg, const char _ack, bool _block){
	strcpy(ip, local_ip);
	strcpy(port, local_port);
	srcID = local_id;
	dstID = _id;
	strcpy(msg, _msg);
	ack = _ack;
	block = _block;
}

RPC::RPC(const char* _ipp, const char* _msg, const char _ack, bool _block){
	strcpy(ip, local_ip);
	strcpy(port, local_port);
	srcID = local_id;
	dstID = SHA_1(_ipp);
	strcpy(msg, _msg);
	ack = _ack;
	block = _block;
}


void* RPC::request(){
	void* retval = 0;
	rpc_mng.push(this);
	pthread_create(&thread_ID, NULL, (THREADFUNCPTR)RPC::requestThread, (void*)this);
	if(block){
		pthread_join(thread_ID, &retval);
		// return retval;
		return retval;
	}else{
		return NULL;
	}
}

void* RPC::requestThread(void * p){
	RPC* rpc = (RPC*)p;
	bool retval = false;
	// (*(bool*)rpc->ret) = false;
	char packet[2048] = "";
	sprintf(packet, "%s:%s:", local_ip, local_port);
	sprintf(packet+strlen(packet), "%s", local_id.get());
    sprintf(packet+strlen(packet), "|%s|%c|",rpc->msg, rpc->ack);
    
    char* pack_ptr = packet+strlen(packet);
    

	if(!strcmp(rpc->msg, "PING")){
		// get node ip and port from the routing table
		Node id = dht.get(rpc->dstID);
		if(id){
			Client_socket csock(id.ip, id.port);
			if(csock){
				sprintf(pack_ptr, "%s|", rpc->dstID.get());
			    csock.send(packet, strlen(packet));
			    printf("<< %s\n", packet);
			    // time threshold
			    time(&rpc->tx_time);
			    time_t now;
			    time(&now);
			    int rtt = abs(int(difftime(now, rpc->tx_time)));
			    // wait for response
			    while((!rpc->response) && (rtt < t_Threshold)){
			    	usleep(1000);
			    	time(&now);
			    	rtt = abs(int(difftime(now, rpc->tx_time)));
			    }
			    printf("\n");
			    if(rpc->response){
			    	// (*(bool*)rpc->ret) = (void*) true;
			    	retval = true;
				    delete rpc->response;
			    }else{
			    	printf("[timeout]\n");
			    }
			}
		}
		
	}else if(!strcmp(rpc->msg, "STORE")){
		Node id = dht.get(rpc->dstID);
		if(id){
			Client_socket csock(id.ip, id.port);
			if(csock){
				sprintf(pack_ptr, "%s|", rpc->dstID.get());
				sprintf(packet+strlen(packet), "%s|", rpc->key.get());

				string sfname = dht.get_file(rpc->key);
				if(sfname != ""){
					char fname[File_size] = "";
					strcpy(fname, shared_folder);
					strcpy(fname + strlen(fname), sfname.c_str());
				    File file(fname, 0);
				    if(file){
				    	strcpy(rpc->name, sfname.c_str());
						rpc->len = file.length();
						int buflen = strlen(packet) + rpc->len;
						char* packet2 = new char [buflen + 1];

						sprintf(packet+strlen(packet), "%s|", rpc->name);
						sprintf(packet+strlen(packet), "%d|", rpc->len);
						csock.send(packet, strlen(packet));
					    printf("<< %s\n", packet);

					    sprintf(packet2, "%s:%s:", local_ip, local_port);
						sprintf(packet2+strlen(packet2), "%s", local_id.get());
					    sprintf(packet2+strlen(packet2), "|%s|%c|",rpc->msg, '1');
					    sprintf(packet2+strlen(packet2), "%s|", rpc->dstID.get());
					    sprintf(packet2+strlen(packet2), "%s|", rpc->key.get());

					    file.read(packet2 + strlen(packet2), rpc->len);
					    packet2[buflen] = '\0';
					    csock.send(packet2, buflen);
					    printf("<< %s\n", packet2);
					    delete [] packet2;

				    }
				}
			}
		}

	}else if(!strcmp(rpc->msg, "FIND_NODE")){
		Node id = dht.get(rpc->dstID);
		if(id){
			Client_socket csock(id.ip, id.port);
			if(csock){
				sprintf(pack_ptr, "%s|", rpc->dstID.get());
				sprintf(packet+strlen(packet), "%s|", rpc->ID.get());
				csock.send(packet, strlen(packet));
			    printf("<< %s\n", packet);
			    // time threshold
			    time(&rpc->tx_time);
			    time_t now;
			    time(&now);
			    int rtt = abs(int(difftime(now, rpc->tx_time)));
			    // wait for response
			    while((!rpc->response) && (rtt < t_Threshold)){
			    	usleep(1000);
			    	time(&now);
			    	rtt = abs(int(difftime(now, rpc->tx_time)));
			    }
			    printf("\n");
			    if(rpc->response){
			    	rpc->rx_time = now;
				    // rpc->response->print();
				    int closer = 0;
				    vector<Node> nods = Node::parse(rpc->response->data, rpc->response->dlen);
				    for(auto& nod : nods){
				    	printf("[back] %s:%s:%s\n", nod.ip, nod.port, nod.ID.get());
				    	if(!dht.contain(nod)){
				    		closer++;
				    	}
				    	if(rpc->param){
				    		((vector<Node>*)rpc->param)->push_back(nod);
				    	}
				    	dht.insert(nod);
				    }
				    delete [] rpc->response->data;
				    delete rpc->response;
				    if(closer){
				    	// (*(bool*)rpc->ret) = (void*) true;
				    	retval = true;
				    	if(rpc->val){
				    		(*(int*)rpc->val) ++;
				    	}

				    }else{
				    	// *rpc->ret = (void*)false;
				    }
			    }else{
			    	printf("[timeout]\n");
			    }	
			}
		}else{
			printf("[Recursive FIND_NODE]\n");
			unordered_map<string, int> seen;
			vector<Node> ids = dht.get_node(rpc->ID);
			int closer = 0;
			bool found = false;
			time(&rpc->tx_time);
			time_t now;
			auto it=ids.begin();
			for(; it!=ids.end() && closer<local_k; ++it){
				string str = string(it->ID.get());
				if(!seen.count(str)){
					RPC* recurs = new RPC(it->ID, "FIND_NODE", '0', false);
	                recurs->ID = rpc->ID;
	                recurs->val = &closer;
	                recurs->param = &ids;
	                recurs->request();
	                seen[str] = 1;
	                usleep(1000);
	            }
	        }
		    time(&now);
		    int rtt = abs(int(difftime(now, rpc->tx_time)));
		    // wait for response
		    while((closer<local_k) && (rtt < t_Threshold+3)){
		    	usleep(1000);
		    	time(&now);
		    	rtt = abs(int(difftime(now, rpc->tx_time)));
		    }
		    printf("\n");
		    if(closer >= local_k){
		    	rpc->rx_time = now;
		    	// (*(bool*)rpc->ret) = (void*) true;
		    	retval = true;
		    }else{
		    	// rpc->ret = (void*)false;
		    	printf("[timeout]\n");
		    }
		}
		
	}else if(!strcmp(rpc->msg, "FIND_VALUE")){
		string sfname = dht.get_file(rpc->key);
		if(sfname != ""){
			// (*(bool*)rpc->ret) = (void*) true;
			retval = true;
		}else{
			Node id = dht.get(rpc->dstID);
			if(id){
				Client_socket csock(id.ip, id.port);
				if(csock){
					sprintf(pack_ptr, "%s|", rpc->dstID.get());
					sprintf(packet+strlen(packet), "%s|", rpc->key.get());
					csock.send(packet, strlen(packet));
				    printf("<< %s\n", packet);
				    // time threshold
				    time(&rpc->tx_time);
				    time_t now;
				    time(&now);
				    int rtt = abs(int(difftime(now, rpc->tx_time)));
				    // wait for response
				    while((!rpc->response) && (rtt < t_Threshold)){
				    	usleep(1000);
				    	time(&now);
				    	rtt = abs(int(difftime(now, rpc->tx_time)));
				    }
				    printf("\n");
				    if(rpc->response){
				    	rpc->rx_time = now;
				    	if(rpc->response->ack == '2'){
				    		// (*(bool*)rpc->ret) = (void*)true;
				    		retval = true;
				    		if(rpc->val){
					    		(*(int*)rpc->val) = true;
					    	}
				    	}else if(rpc->response->ack == '1'){
						    vector<Node> nods = Node::parse(rpc->response->data, rpc->response->dlen);
						    for(auto& nod : nods){
						    	printf("[back] %s:%s:%s\n", nod.ip, nod.port, nod.ID.get());
						    	if(rpc->param){
						    		((vector<Node>*)rpc->param)->push_back(nod);
						    	}
						    	dht.insert(nod);
						    }
					    	delete [] rpc->response->data;
					    	delete rpc->response;
					    }
				    }else{
				    	printf("[timeout]\n");
				    }
				}	
			}else{
				printf("[Recursive FIND_VALUE]\n");
				vector<Node> ids = dht.get_node(rpc->key);
				unordered_map<string, int> seen;
				bool found = false;
				time(&rpc->tx_time);
				time_t now;
				auto it=ids.begin();
				for(; it!=ids.end() && !found; ++it){
					string str = string(it->ID.get());
					if(!seen.count(str)){
						RPC* recurs = new RPC(it->ID, "FIND_VALUE", '0', false);
		                recurs->key = rpc->key;
		                recurs->val = &found;
		                recurs->param = &ids;
		                recurs->request();
		                seen[str] = 1;
		                usleep(1000);
		            }
		        }
		        time(&now);
			    int rtt = abs(int(difftime(now, rpc->tx_time)));
			    // wait for response
			    while((!found) && (rtt < t_Threshold+3)){
			    	usleep(1000);
			    	time(&now);
			    	rtt = abs(int(difftime(now, rpc->tx_time)));
			    }
			    printf("\n");
			    if(found){
			    	rpc->rx_time = now;
			    	// (*(bool*)rpc->ret) = (void*) true;
			    	retval = true;
			    }else{
			    	// rpc->ret = (void*)false;
			    	printf("[timeout]\n");
			    }
			}
			
		}
	}else{
		printf("else\n");
	}

	rpc_mng.remove(rpc);
	if(!rpc->block){
		delete rpc;
	}
	return (void*)retval;
	// return NULL;
}

void* RPC::respond(){
	rpc_mng.push(this);
	pthread_create(&thread_ID, NULL, (THREADFUNCPTR)RPC::respondThread, (void*)this);
	if(block){
		// pthread_join(thread_ID, ret);
		// (*(bool*)rpc->ret) = (void*) true;
		// return ret;
		return NULL;
	}else{
		return NULL;
	}
}

void* RPC::respondThread(void * p){

	RPC* rpc = (RPC*)p;
	// needs response
	char packet[2048] = "";
	sprintf(packet, "%s:%s:", local_ip, local_port);
	sprintf(packet+strlen(packet), "%s", local_id.get());
    sprintf(packet+strlen(packet), "|%s|%c|",rpc->msg, '1');

    char* ack_ptr = packet+strlen(packet) - 2;
    sprintf(packet+strlen(packet), "%s|", rpc->srcID.get());

    Client_socket csock(rpc->ip, rpc->port);
    if(csock){
    	printf("%s:%s\n", rpc->ip, rpc->port);
		if(!strcmp(rpc->msg, "PING")){
			csock.send(packet, strlen(packet));
		    printf("<< %s\n", packet);

		}else if(!strcmp(rpc->msg, "STORE")){
			// time threshold
		    time(&rpc->tx_time);
		    time_t now;
		    time(&now);
		    int rtt = abs(int(difftime(now, rpc->tx_time)));
		    // wait for response
		    while((!rpc->response) && (rtt < t_Threshold)){
		    	usleep(1000);
		    	time(&now);
		    	rtt = abs(int(difftime(now, rpc->tx_time)));
		    }
		    printf("\n");
		    if(rpc->response){
		    	// rpc->response->print();
		    	char fname[File_size] = "";
				strcpy(fname, shared_folder);
				strcpy(fname + strlen(fname), rpc->name);
			    File file(fname, 1);
			    file.write(rpc->response->data, rpc->len);
			    dht.add_file(rpc->key, rpc->name);
			    delete [] rpc->response->data;
			    delete rpc->response;
		    }else{
		    	printf("[timeout]\n");
		    }

		}else if(!strcmp(rpc->msg, "FIND_NODE")){
			// get node ip and port from the routing tree
		    vector<Node> ids = dht.get_node(rpc->ID);
		    if(ids.size()){
		    	sprintf(packet+strlen(packet), "%s|", rpc->ID.get());
			    for(auto& id : ids){
			    	sprintf(packet+strlen(packet), "%s:%s:%s,", id.ip, id.port, id.ID.get());
			    }
			    packet[strlen(packet)-1] = '\0';
				csock.send(packet, strlen(packet));
			    printf("<< %s\n", packet);
		    }
		}else if(!strcmp(rpc->msg, "FIND_VALUE")){
			string sfname = dht.get_file(rpc->key);
			if(sfname != ""){
				*ack_ptr = '2';
				csock.send(packet, strlen(packet));
		    	printf("<< %s\n", packet);
				RPC* store = new RPC(rpc->srcID, "STORE", '0', false);
				store->key = rpc->key;
				store->request();
			}else{
				vector<Node> ids = dht.get_node(rpc->key);
			    if(ids.size()){
			    	sprintf(packet+strlen(packet), "%s|", rpc->key.get());
				    for(auto& id : ids){
				    	sprintf(packet+strlen(packet), "%s:%s:%s,", id.ip, id.port, id.ID.get());
				    }
				    packet[strlen(packet)-1] = '\0';
					csock.send(packet, strlen(packet));
				    printf("<< %s\n", packet);
			    }
			}
		}else{
			printf("else\n");
		}
	}
	dht.insert(Node(rpc->ip, rpc->port, rpc->srcID));

	rpc_mng.remove(rpc);
	if(!rpc->block){
		delete rpc;
	}
	return nullptr;
}

void RPC::print() const {
	printf("ip    : %s\n", ip);
	printf("port  : %s\n", port);
	printf("srcID : %s\n", srcID.get());
	printf("msg   : %s\n", msg);
	printf("ack   : %c\n", ack);
	printf("dstID : %s\n", dstID.get());
	printf("key   : %s\n", key.get());
	printf("data  : %s\n", data);
}

bool RPC::match(const RPC* _a, const RPC* _b){
	// printf("\n\na:\n");
	// _a->print();
	// printf("\n\nb:\n");
	// _b->print();
	
	if(	(_a->srcID == _b->dstID) && (_a->dstID == _b->srcID) &&
	 	!strcmp(_a->msg, _b->msg) && (_a->ack != _b->ack)){
		return true;

	}else if(	(_a->srcID == _b->srcID) && (_a->dstID == _b->dstID) && 
				!strcmp(_a->msg, "STORE") && !strcmp(_b->msg, "STORE") && 
				(_a->ack != _b->ack) && (_a->key == _b->key)){
		return true;

	}else{
		return false;
	}
}

// ==========================================================================================
// RPC_Manager::
// ==========================================================================================
void RPC_Manager::handle(const char* _buf, const int _len){

	RPC* rpc = resolve(_buf, _len);
	if(!rpc){
		printf("[drop]\n");
		return;
	}
	if(local_id != rpc->dstID){
		// discard
		printf("[drop]\n");
	}else{
		// printf("rpc received.\n");
		if(rpc->ack == '0'){
			rpc->respond();
		}else{
			RPC* it = 0;
			for(auto& _r : RPC_list){
				if(RPC::match(_r, rpc)){
					printf("[match]\n");
					_r->response = rpc;
					it = _r;
					break;
				}
			}
			if(!it){
				printf("[no match]\n");
				delete rpc;
			}
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
			ret->block = true;
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
		if(len >= 0){
			ret->dlen = len;
			ret->data = new char [len];
			memcpy(ret->data, pos, len);
			// printf("new [%d]\n", len);
		}
	}
	// ret->print();
	return ret;
}

void RPC_Manager::push(RPC* _rpc){
	RPC_list.push_back(_rpc);
	printf("[push] size: %d\n", int(RPC_list.size()));
}

void RPC_Manager::remove(RPC* _rpc){
	for(auto it=RPC_list.begin(); it!=RPC_list.end(); ++it){
		if(*it == _rpc){
			RPC_list.erase(it);
			printf("[pop] size: %d\n", int(RPC_list.size()));
			break;
		}
	}
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
			recvbuf[n] = '\0';
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
		dht.join();
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
