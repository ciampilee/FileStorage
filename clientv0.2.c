#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/un.h>
#include"projectutils/projectutils.h"

int main(){
	//1. creazione del file descriptor
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);

	//2. creazione e setting della struct del path del file descriptor
	struct sockaddr_un addr;//ricordiamo la struttura: {sun_family, sun_path[108]}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "socket", sizeof(addr.sun_path)-1);

	//3. connessione al server
	connect(fd, (struct sockaddr*)&addr, sizeof(addr));

	//4. invio del file
	parsedconfig config = parseconfig("config.txt");
	int bufsz = config.clientbufsz;
	FILE* fp = fopen("config.txt", "r");

	sock_sendfile(fd, fp, bufsz);
	
}