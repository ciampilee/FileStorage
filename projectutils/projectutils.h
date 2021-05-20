/* 	parsedconfig: oggetto di tipo struct parsedfile contenente le informazioni 
	relative alla configurazione del server descritte sotto */
typedef struct parsedfile{
	int threadnum; 	//numero di thread workers
	int memsize;	//dimensione della memoria
	char sockfile[30];	//path del file di socket
	char logfile[30];	//path del file di log
} parsedconfig;

/*	mysock: oggetto di tipo struct mysock contenente la coppia {sock_fd, sock_cl} dove
	fd è il file descriptor della socket e cl è l'intero restituito dalla accept identificando
	quindi il client */
typedef struct mysock{
	int sock_fd; //socket file descriptor
	int sock_cl;	//identificatore del client
} mysock;

parsedconfig parseconfig(char*);
int unixServerSocket(char*, int);
int unixClientSocket(char*);
