#include "kad_util.hpp"

bool RUNNING = true;

char config_file[200] = "default.config";
char bootstrap[32] = "";
char local_port[32] = "";
char local_ip[32] = "";
int local_k = 0;


void* serverThread(void* p){

	Server_socket server(local_port);
	if(!server.valid()){
		cerr << "Server initialization failed.\n";
		RUNNING = false;
		return 0;
	}
	printf("local ip  : %s\n", server.get_ip());

	struct pollfd fds[20];
	memset(fds, 0 , sizeof(fds));
	int    nfds = 1;
	fds[0].fd = server.get_sock();
	fds[0].events = POLLIN;
	int ptimeout = 1;
	int retVal = 0;
	
	cout << "Waiting..." << endl;
	while(RUNNING){
		if((retVal = poll(fds, nfds, ptimeout)) > 0){
			server.accept();
		}else{
			usleep(500);
		}
		
	}
	return 0;
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
