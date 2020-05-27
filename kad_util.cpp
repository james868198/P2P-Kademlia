#include "kad_util.hpp"

bool RUNNING = true;

char config_file[256] = "default.config";
char bootstrap[32] = "";
char local_port[32] = "";
char local_ip[32] = "";
SHA_1 local_id;
int local_k = 0;
int local_alpha = 0;
char shared_folder[256] = "Shared/";
char download_folder[256] = "Download/";

SHA_1::SHA_1(char* _str){
	strcpy(str, _str);
	SHA1((unsigned char*)str, strlen(str), (unsigned char*)hash);
	for(int i=0; i<SHA_DIGEST_LENGTH; i++){
        sprintf(key+strlen(key), "%02x", hash[i]);
    }
}

bool SHA_1::operator == (const SHA_1& a){
	int i = SHA_DIGEST_LENGTH;
	while(i--){
		if(a.hash[i-1] != this->hash[i-1]){
			return false;
		}
	}
	return true;
}

RPC::RPC(const SHA_1 _id, const char* _msg, const char _ack){
	id = _id;
	strcpy(msg, _msg);
	ack = _ack;
}

void RPC::request(){
	char packet[512] = "";
	sprintf(packet, "%s:%s:", local_ip, local_port);
	sprintf(packet+strlen(packet), "%s", local_id.get());
    sprintf(packet+strlen(packet), "|PING|%c|", ack);
    sprintf(packet+strlen(packet), "%s", id.get());

    // get node ip and port from the routing tree
    Client_socket node("192.168.1.37", "8899");
    node.send(packet, strlen(packet));

    printf("<< %s\n", packet);
}

void RPC::response(){

}


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

		}else{
			usleep(500);
		}
		
	}
	return 0;
}

bool exists(const fs::path& p, fs::file_status s = fs::file_status{}){
    std::cout << p;
    if(fs::status_known(s) ? fs::exists(s) : fs::exists(p)){
        std::cout << " exists\n";
        return true;
    }else{
        std::cout << " does not exist\n";
        return false;
    }
}

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
		if(!fs::exists(shared_folder)){
			fs::create_directories(shared_folder);
		}
		if(!fs::exists(download_folder)){
			fs::create_directories(download_folder);
		}
		
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
