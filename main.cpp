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
	usleep(100000);

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
			printf("ping %s ---- %s!\n", pos, (ret ? "Yes" : "No"));
			delete rpc;

		}else if(!strcmp(cmd, "store")){
			// e.g. store file1.txt 192.168.1.37:8888
			char fname[File_size] = "";
			tok = strstr(pos, " ");
			if(tok){
				n = tok - pos;
				strncpy(fname, pos, n);	pos += n+1;
				SHA_1 fid = SHA_1(fname);
				// bool ret = 0;
				RPC* rpc = new RPC(pos, "STORE", '0', false);
				// rpc->ret = &ret;
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
			// rpc->ret = &ret;
			ret = rpc->request();
			delete rpc;
			printf("get %s ---- %s!\n", fname, ((bool)ret ? "Yes" : "No"));
			
		}
	}
}






