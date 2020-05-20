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
	
	//get config file content
	if(!get_config(config_file)){
		return 1;
	}

	// create tcp server
	pthread_t server_ID = 0;
	pthread_create(&server_ID, NULL, serverThread, NULL);
	usleep(100000);

	printf("-----------------------------------\n");
	printf("Waiting for command:\n");
	string cmd = "";
	while(RUNNING){
		cin >> cmd;
		if(cmd == "exit"){
			RUNNING = false;
			pthread_join(server_ID, NULL);
			break;
		}
		usleep(500);
	}

	printf("\n\n.\n");
	return 0;
}

