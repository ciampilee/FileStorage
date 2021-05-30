#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/un.h>
#include<pthread.h>
#include"projectutils/projectutils.h"
#define SIZE 32

parsedconfig config;

void* connectionhandler(void* cl_p){
	int cl = *((int*)cl_p);
	printf("client socket %d accepted.\n", cl);

	char path[20] = "Files/receivedi.txt\0";
	path[14]=cl+'0';
	
	write_file(path, cl, &config);

	close(cl);
	return NULL;
}


int main(){
	config = parseconfig("config.txt");
	int fd = unixSocketServer(config.sockfile);
	setStoredSize(NULL, &config);

	while(1){
		int cl = accept(fd, NULL, NULL);
		if(cl != 0){
			int cla = cl;
			pthread_t t;
			pthread_create(&t, NULL, connectionhandler, &cla);
		}
		cl = 0;
	}
}