#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/un.h>
#include"projectutils.h"

/*	parametri: char* filepath = indirizzo del file
	restituisce un oggetto di tipo parsedconfig */
parsedconfig parseconfig(char* filepath){ 
	if(filepath == NULL)
		filepath = "config.txt";

	parsedconfig parsed;
	char trash[20];
	FILE* fp = fopen(filepath, "r");

	if(fp == NULL){
		printf("configuration file not found\n");
		return parsed;
	}

	fscanf(fp, "%s = %d", trash, &(parsed.threadnum));
	fscanf(fp, "%s = %d", trash, &(parsed.memsize));
	fscanf(fp, "%s = %s", trash, parsed.sockfile);
	fscanf(fp, "%s = %s", trash, parsed.logfile);
	fscanf(fp, "%s = %d", trash, &(parsed.clientbufsz));
	fscanf(fp, "%s = %d", trash, &(parsed.serverbufsz));

	printf("debug : config file has been parsed successfully.\n");
	fclose(fp);

	return parsed;
}


/* 	parametri:	char* fd_path = indirizzo del file descriptor scelto
				int max_conectn = massimo numero di connessioni accettate 
				(si presuppone provenga dal config.txt)
	restituisce un file descriptor già bindato ed in ascolto */
int unixServerSocket(char* fd_path, int max_conectn){
	printf("debug : socketpath = %s\n", fd_path);
	unlink(fd_path);
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un addr;

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, fd_path, sizeof(addr.sun_path));
	bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	listen(fd, max_conectn);
	return fd;
}


/* 	parametri:	char* fd_path = indirizzo del file descriptor scelto
	restituisce un file descriptor già collegato */
int unixClientSocket(char* fd_path){
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un addr;

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, fd_path);
	connect(fd, (struct sockaddr*)&addr, sizeof(addr));
	return fd;
}


/*	parametri: char* path = indirizzo del file da misurare
	restituisce la dimensione del file oppure -1 in caso di errore*/
long filesize(FILE* fp){
	long zero = 0;
	if(!fseek(fp, zero, SEEK_END))
		return ftell(fp);
	else return (long) -1;
}


/*	parametri:	int fd = file descriptor della socket su cui si vuole inviare il file
				FILE* input_file = puntatore al file che si vuole inviare
				int buffersize = dimensione del buffer che si vuole utilizzare
	restituisce la dimensione del file oppure -1 in caso di errore*/
void sock_sendfile(int fd, FILE* input_file, int buffersize){
	void* buffer = malloc(sizeof(char)*100);

	while (1) {
		int bytes_read = read(fileno(input_file), buffer, sizeof(buffer));
		if (bytes_read == 0) //se nessun byte viene letto, il file è terminato.
			break;
		
		if (bytes_read < 0) {
			//TODO gestione errori
		}

		// per la write si necessita un loop, non tutti i byte potrebbero essere scritti
		// in una chiamata. La write restituisce il numero di byte inviati. p mantiene
		// la posizione corrente all'interno del buffer, si decrementa bytes_read
		// per tenere traccia dei byte che rimangono da scrivere.
		void *p = buffer;
		while (bytes_read > 0) {
			int bytes_written = write(fd, p, bytes_read);
			if (bytes_written <= 0){
			// handle errors
			}

			bytes_read -= bytes_written;
			p += bytes_written;
		}
	}
}