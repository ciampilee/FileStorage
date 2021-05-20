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
