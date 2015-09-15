#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include "httpserver.h"

int main(){
	//initializeServer(8097);
	char PORT[6];
	strcpy(PORT, "8097");
	initializeServer(PORT);
	return 0;
}