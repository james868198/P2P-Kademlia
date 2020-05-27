// Author: Yu-Cheng Chen, Yu-Chan Huang
// Date: 5/19/2020
// Contact: ychen22@scu.edu
// CEON 317 Distributed System
// P2P-Kademlia

#include "kad_util.hpp"


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

	// create tcp server
	pthread_t server_ID = 0;
	pthread_create(&server_ID, NULL, serverThread, NULL);
	usleep(100000);

	// printf("-----------------------------------\n");
	printf("Waiting for command:\n");
	char cmd[1024] = "";
	
	while(RUNNING){
		cin.getline(cmd, 1024);
		// cin >> cmd;
		// handle the command 
		// if(cmd == "exit"){
		if(!strcmp(cmd, "exit")){
			RUNNING = false;
			pthread_join(server_ID, NULL);
			break;
		}else{
			// char ip[32] = "192.168.1.20";
			// char port[32] = "8888";
			// char sendbuf[1400] = "Hello from "; 
			// sprintf(sendbuf+strlen(sendbuf), "%s:%s", local_ip, local_port);
			// sscanf(cmd, "%s %s %s", ip, port, sendbuf);
			// Client_socket client(ip, port);
			// if(client){
			// 	client.send(sendbuf, strlen(sendbuf));
			// }else{
			// 	printf("hostname not found\n");
			// }
			char str[64] = "";
			sprintf(str, "%s:%s", bootstrap, local_port);
			SHA_1 id(str);
			RPC node(id, "PING", '0');
			node.request();
		}
		usleep(500);
	}

	printf("\n\ndone.\n");
	return 0;
}

