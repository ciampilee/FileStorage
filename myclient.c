#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/un.h>
#include"projectutils/projectutils.h"
#define SIZE 32

int main(){
	int fd = unixSocketClient("socket");
	printf("sock = %d\n", fd);
	send_file("myclient.c", fd);
	close(fd);

    return 0;
}

