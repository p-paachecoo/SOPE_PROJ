

int fd_server;
int identifier_c = 1;
char* server_path;

struct message {
	int rid;	// request id
	pid_t pid;	// process id
	pthread_t tid;	// thread id
	int tskload;	// task load
	int tskres;	// task result
};

void createRequests();

void* makeRequest();

void log_msg(int rid, pid_t pid, pthread_t tid, int tskload, int tskres, char *operation);