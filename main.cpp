// Author: Yu-Cheng Chen, Yu-Chan Huang
// Date: 5/19/2020
// Contact: ychen22@scu.edu
// CEON 317 Distributed System
// P2P-Kademlia

#include "main.hpp"

char config_file[200] = "default.config";
char bootstrap[32] = "";
char local_port[32] = "";
char local_ip[32] = "";
int local_k = 0;

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
	
	//get config file content
	ifstream config;
	config.open(config_file);
	if(!config){
	    cerr << "Cannot open config file.\n";
	    return 1;
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
			}else{
				;
			}
		}
		config.close();
		printf("-----------------------------------\n");
		printf("    Configurations      \n");
		printf("-----------------------------------\n");
		printf("bootstrap : %s\n", bootstrap);
		printf("port      : %s\n", local_port);
		printf("k         : %d\n", local_k);
	}

	// create tcp server
	Server_socket server(local_port);
	if(!server.valid()){
		cerr << "Server initialization failed.\n";
		return 1;
	}
	printf("local ip  : %s\n", server.get_ip());
	printf("-----------------------------------\n");
	printf("Waiting for command:\n");
	string cmd = "";
	while(true){
		cin >> cmd;
		if(cmd == "exit"){
			break;
		}
		usleep(500);
	}

	printf("\n\n.\n");
	return 0;
}

