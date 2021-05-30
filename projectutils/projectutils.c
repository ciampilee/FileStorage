#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/un.h>
#include<dirent.h>
#include"projectutils.h"
#define SIZE 32

/*	parametri: char* filepath = indirizzo del file
	restituisce un oggetto di tipo parsedconfig (definizione in projectutils.h)*/
parsedconfig parseconfig(char* filepath){ 
	if(filepath == NULL)
		filepath = "config.txt";

	parsedconfig parsed;
	char trash[20];
	FILE* fp = fopen(filepath, "r");

	if(fp == NULL){
		perror("[-]Error. Configuration file not found. ");
		return parsed;
	}

	fscanf(fp, "%s = %d", trash, &(parsed.threadnum));
	fscanf(fp, "%s = %d", trash, &(parsed.memsize));
	fscanf(fp, "%s = %s", trash, parsed.sockfile);
	fscanf(fp, "%s = %s", trash, parsed.logfile);
	fscanf(fp, "%s = %d", trash, &(parsed.sockbufsz));

	//DEBUG printf("debug : config file has been parsed successfully.\n");
	fclose(fp);

	return parsed;
}


/*	parametri: char* path = indirizzo del file da misurare
	restituisce la dimensione del file oppure -1 in caso di errore*/
int filesize(char* filepath){
	FILE* fp = fopen(filepath, "r");
	if(fp == NULL) return -2; //se fp è NULL è una cartella

	if(!fseek(fp, (long)0, SEEK_END))
		return (int) ftell(fp);

	else return -1;
}


/*	parametri: char* dirpath = indirizzo della cartella di cui si vuole conoscere la dimensione
	restituisce la dimensione della cartella a cui appartiene dirpath (int)
	FUNZIONE RICORSIVA*/
int dirsize(char* entpath){
	//DEBUG : printf("debug : entering directory %s\n", entpath);
	DIR* d = opendir(entpath);
	struct dirent* ent;
	int size = 0;
	int entsize;

	if(d == NULL){
		//DEBUG printf("debug : avoiding %s\n", entpath);
		return 0;
	}

	while(ent = readdir(d)){
		char newent[30];
		strcpy(newent, entpath);
		strcat(newent, "/");
		strcat(newent, ent->d_name);
		//DEBUG printf("debug : found possible file : %s", newent);

		entsize = filesize(newent);
		//DEBUG printf(" (%d)\n", entsize);

		if(entsize > 0) size += entsize;

		else if(entsize < 0 && !strchr(newent, '.')){
			//DEBUG printf("debug : new entpath : %s\n", newent);
			size += 4100 + dirsize(newent); //4100 è la dimensione occupata da una cartella vuota
		}
	}

	return size;
}


/*	funzione di utilità per modificare la variabile di tracciamento dell'occupazione della memoria*/
int setStoredSize(int* x, parsedconfig* config){
	int dim;

	if(x == NULL){
		dim = dirsize("Files");
		config->storedmem = dim;
	}

	else{
		//DEBUG printf("debug : incoming file size : %d\n", *x);
		dim = config->storedmem;
		//DEBUG printf("debug : requested to add %d to Stored memsize (%d) = %d\n", *x, dim, *x + dim);
		dim = *x + dim;
		//DEBUG printf("%d\n", dim);
		config->storedmem += *x;
	} 

	//DEBUG printf("debug : updated stored size to %d\n", dim);
	return dim;
}


/*	funzione di utilità per la ricezione di un file: verifica la sua dimensione prima della ricezione
	parametri : int sockfd = file descriptor della socket su cui ricevere la dimensione
				parsedconfig config = file di configurazione parsato
	restituisce 1 se la dimensione è accettabile e 0 altrimenti*/
int FileSizeCheck(int sockfd, parsedconfig* config){
	int premem;						//previsione della dimensione della memoria a fine operazione
	int* buf = malloc(sizeof(int));	//buffer per la ricezione della dimensione del file
	int* ans = malloc(sizeof(int));	//flag per l'accettazione del file

	//valori di default
	*buf = 0;
	*ans = 1;

	read(sockfd, buf, sizeof(int)); //lettura dimensione file
	//DEBUG printf("debug : incoming file size : %d\n", *buf);
	premem = config->storedmem + *buf;	//calcolo dimensione memoria a fine operazione
	//DEBUG printf("debug : predicted size : %d\n", premem);

	//rifiuto del file in caso la capienza sia insufficiente
	if(premem > config->memsize){
		perror("[-]Error. Server Ovf. \n");
		ans = 0;
		write(sockfd, &ans, sizeof(int));
		return 0;
	}

	
	write(sockfd, &ans, sizeof(int)); //accettazione del file in caso la capienza sia sufficiente
	setStoredSize(buf, config); //aggiornamento della memoria TODO: passare la responsabilità ad un thread
	return 1;
}


/*	parametri:	FILE* filepath = puntatore al file che si vuole inviare
				int sockfd = file descriptor della socket su cui si vuole inviare il file
	restituisce 0 oppure -1 in caso di errore*/
void send_file(char* filepath, int sockfd){
	char data[SIZE];
	int* ans = malloc(sizeof(int));
	int* size = malloc(sizeof(int));
	*ans = 0;
	*size = filesize(filepath);
	FILE* fp = fopen(filepath,"r");

	//DEBUG printf("filesize : %d\n", *size);
	write(sockfd, size, sizeof(int));
	read(sockfd, ans, sizeof(int));

	if(*ans == 0){
		perror("[-]Error. Server Ovf. ");
		exit(1);
	}

		while(fgets(data, SIZE, fp) != NULL) {
			if (send(sockfd, data, sizeof(data), 0) == -1) {
				perror("[-]Error in sending file. ");
				exit(1);
			}
			bzero(data, SIZE);
		}
}

/*	parametri:	char* filepath = nome del file che viene inviato.
				int sockfd = file descriptor della socket su cui viene inviato il file. 
				parsedconfig config = file di configurazione parsato*/
void write_file(char* filepath, int sockfd, parsedconfig* config){
	if(!FileSizeCheck(sockfd, config)) return; //controllo per la capienza

	//controllo dell'esistenza del file e rinominazione di questo
	//TODO: rinominare il file con un semplice append finché il nome non è disponibile
 	if(access(filepath, F_OK) == 0){
    	perror("[-]Error. File already exists. ");
    	//exit(2);
    	filepath[14] += 2;
  	}

  	FILE *fp = fopen(filepath, "w");
  	char buffer[SIZE];

  	//ricezione del file
  	int n; //byte ricevuti
  	while (1) {
    	n = recv(sockfd, buffer, SIZE, 0);
    	if (n <= 0){
      		fclose(fp);
      		break;
      		return;
    	}
    	fprintf(fp, "%s", buffer);
    	bzero(buffer, SIZE);
    }
    return;
}


/* 	parametri:	char* sockpath = indirizzo del file descriptor scelto
	restituisce il file descriptor (int) della socket creata già bindato ed in ascolto */
int unixSocketServer(char* sockpath){
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un addr;

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path)-1);
	unlink(addr.sun_path);

	bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	listen(fd, 1);

	return fd;
}

/* 	parametri:	char* sockpath = indirizzo del file descriptor scelto
	restituisce il file descriptor (int) della socket collegata */
int unixSocketClient(char* sockpath){
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	struct sockaddr_un addr;

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path)-1);
	connect(fd, (struct sockaddr*)&addr, sizeof(addr));

	return fd;
}