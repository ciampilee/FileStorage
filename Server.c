#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include"projectutils/projectutils.h"

void* connectionhandler();

int main(){
	parsedconfig config = parseconfig("config.txt");

	int fd = unixServerSocket(config.sockfile, config.threadnum);
	int cl;

	while(1){
		printf("debug : awaiting connections ...\n");
		
		cl = accept(fd, NULL, NULL);
		if(cl > 0){
			pthread_t t;
			pthread_create(&t, NULL, connectionhandler, &cl);
		}
	}

	return 0;
}


void* connectionhandler(void* pcl){
	int cl = *((int*)pcl);
	printf("debug : connection established. connection number = %d\n", cl);
	//free(pcl);

	char buf[100];
	int rcv;

	while(rcv = read(cl, buf, sizeof(buf)))
		printf("CLIENT said :%.*s\n", rcv, buf);

	close(cl);
	return NULL;
}