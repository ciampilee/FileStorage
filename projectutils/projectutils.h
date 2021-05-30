#include<stdio.h>

/* 	parsedconfig: oggetto di tipo struct parsedfile contenente le informazioni 
	relative alla configurazione del server descritte sotto */
typedef struct parsedfile{
	int threadnum; 	//numero di thread workers
	int memsize;	//dimensione della memoria
	int storedmem;	//path del file di tracciamento dello storage
	char sockfile[30];	//path del file di socket
	char logfile[30];	//path del file di log
	int sockbufsz;
} parsedconfig;

parsedconfig parseconfig(char*);
int filesize(char*);
int dirsize(char*);
int setStoredSize(int*, parsedconfig*);
void write_file(char*, int, parsedconfig*);
void send_file(char*, int);
int getSendFlag(int, int);
void setSendFlag(int, int, int);
int unixSocketServer(char*);
int unixSocketClient(char*);
