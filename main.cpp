// Author: Yu-Cheng Chen, Yu-Chan Huang
// Date: 5/19/2020
// Contact: ychen22@scu.edu
// CEON 317 Distributed System
// P2P-Kademlia

#include "kad_util.hpp"
#include "kad_bucket.hpp"

// ==========================================================================================
// global objects
// ==========================================================================================
RPC_Manager rpc_mng;
DHT dht;
Server_socket server;


void cmd_handle(const char* _cmd);

int main(int argc, char* argv[]){
	
	//get command line arguments
	for(int i=1; i<argc; i++){
		string arg = string(argv[i]);
		if(arg == "-h" || arg == "-help"){
			help();
			return 0;
		}
		if(arg == "-c" || arg == "-config"){
			if(i + 1 < argc){
				strcpy(config_file, argv[++i]);
			}else{ 
				std::cerr << "-config option requires one argument." << std::endl;
				return 1;
			}  
		}
	}
	printf("==================================================\n");
	printf("            Welcome to P2P-Kademlia\n");
	printf("==================================================\n");

	//print GMT time
	char gtime[30] = "";
	print_time(gtime);
	printf("%s\n", gtime);
	
	// get config file content
	if(!get_config(config_file)){
		return 1;
	}
	// create udp server
	pthread_t server_ID = 0;
	pthread_create(&server_ID, NULL, serverThread, NULL);
	usleep(10000);
	
	// printf("-----------------------------------\n");
	printf("Waiting for command:\n");
	char cmd[1024] = "";
	
	while(RUNNING){
		cin.getline(cmd, 1024);
		if(!strcmp(cmd, "exit")){
			RUNNING = false;
			break;
		}else{
			cmd_handle(cmd);
		}
		usleep(500);
	}

	printf("\n\ndone.\n");
	return 0;
}

void cmd_handle(const char* _cmd){
	char* pos = (char*)_cmd;
	char* tok = 0;
	int n = 0;
	tok = strstr(pos, " ");
	if(!tok){
		if(!strcmp(_cmd, "ls")){
			dht.print_file();
		}else if(!strcmp(_cmd, "getBuckets")){
			printf("%08.0f [bucket list] ", time_stamp());
			dht.print_all();

		}else if(!strcmp(_cmd, "getAddr")){
			// e.g. getAddr
			printf("%08.0f [local address] %s:%s\n", time_stamp(), local_ip, local_port);
			
		}else{

		}
	}else{
		n = tok - pos;
		char cmd[1024] = "";
		strncpy(cmd, pos, n); 	pos += n+1;

		if(!strcmp(cmd, "ping")){
			// e.g. ping 192.168.1.37:8888
			bool ret = 0;
			RPC* rpc = new RPC(pos, "PING", '0', true);
			rpc->ret = &ret;
			ret = rpc->request();
			printf("%08.0f [ping] %s --- %s %8.4fs\n", time_stamp(), pos, (ret ? "Yes" : "No"), rpc->rtt);
			delete rpc;

		}else if(!strcmp(cmd, "store")){
			// e.g. store file1.txt 192.168.1.37:8888
			char fname[File_size] = "";
			tok = strstr(pos, " ");
			if(tok){
				n = tok - pos;
				strncpy(fname, pos, n);	pos += n+1;
				SHA_1 fid = SHA_1(fname);
				RPC* rpc = new RPC(pos, "STORE", '0', false);
				rpc->key = fid;
				rpc->request();
			}

		}else if(!strcmp(cmd, "get")){
			// e.g. get file1.txt
			char fname[File_size] = "";
			strcpy(fname, pos);
			SHA_1 fid = SHA_1(fname);
			bool ret = 0;
			RPC* rpc = new RPC("", "FIND_VALUE", '0', true);
			rpc->key = fid;
			ret = rpc->request();
			printf("%08.0f [get] %s --- %s %8.4fs\n", time_stamp(), fname, ((bool)ret ? "Yes" : "No"), rpc->rtt);
			delete rpc;
			
		}else if(!strcmp(cmd, "getBucket")){
			// e.g. getAddr
			int num = atoi(pos);
			printf("%08.0f [bucket] %3d:", time_stamp(), num);
			dht.print_buck(num);

		}else if(!strcmp(cmd, "join")){
			// e.g. join 192.168.1.37:8888
			SHA_1 id(pos);
			RPC* rpc = new RPC(id, "FIND_NODE", '0', false);
			rpc->ID = local_id;
			rpc->request();
		}
	}
}






