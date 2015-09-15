//#include "httpserver.h"
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define MAX_CONNECTIONS 1000
#define MAX_SUPPORTED_TYPES 250
#define BUFFER_SIZE 1024
#define MAX_RETRIES 100

int clients[MAX_CONNECTIONS];
char *supported_types[MAX_SUPPORTED_TYPES];
char *PORT;

char *get_filename_ext(char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void respond(int n)
{
	char *root;
	const char *extension;
	root = getenv("PWD");
	printf("Responding to requests on socket %d\n\n", clients[n]);
    char mesg[99999], *reqline[3], data_to_send[BUFFER_SIZE], path[99999];
    int request, fd, bytes_read;

    memset( (void*)mesg, (int)'\0', 99999 );

    request=recv(clients[n], mesg, 99999, 0);

    if (request<0)    // receive error
        fprintf(stderr,("recv() error\n"));
    else if (request==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
    {
        reqline[0] = strtok (mesg, " \t\n");
        if ( strncmp(reqline[0], "GET\0", 4)==0 )
        {
            reqline[1] = strtok (NULL, " \t");
            reqline[2] = strtok (NULL, " \t\n");
            if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
            {
            	char *bad_request_str = "HTTP/1.1 400 Bad Request: Invalid HTTP-Version: %s\n", reqline[2];
            	int bad_request_strlen = strlen(bad_request_str);
                write(clients[n], bad_request_str, bad_request_strlen);
            }
            else
            {
                if ( strncmp(reqline[1], "/\0", 2)==0 )
                    reqline[1] = "/index.html"; //Client is requesting the root directory, send it index.html

                strcpy(path, root);
                strcpy(&path[strlen(root)], reqline[1]); //creating an absolute filepath

                if ( (fd=open(path, O_RDONLY))!=-1 )
                {
                    send(clients[n], "HTTP/1.1 200 OK\n\n", 17, 0);
                    while ( (bytes_read=read(fd, data_to_send, BUFFER_SIZE))>0 )
                        write (clients[n], data_to_send, bytes_read);                    	
                }
                else{
                	char *not_found_str = "HTTP/1.1 404 Not Found %s\n", path;
                	int not_found_strlen = strlen(not_found_str);
                	write(clients[n], not_found_str, not_found_strlen); //file not found, send 404 error
                }   
            }
        }
    }

    shutdown (clients[n], SHUT_RDWR);
    close(clients[n]);
    clients[n]=-1;
}

void initializeServer(char *port){

	int create_socket, new_socket;
	socklen_t addrlen;
	char *buffer = malloc(BUFFER_SIZE);
	struct sockaddr_in address;

	int i = 0;
	for(i; i<MAX_CONNECTIONS; i++){
		clients[i] == -1;
	}

	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){
		printf("Socket created successfully\n");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8097);

	int yes = 1;
	if ( setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
	{
	    perror("setsockopt");
	}
	int num_try = 0;
	while(num_try < MAX_RETRIES){
		if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){
			printf("Binding socket\n");
			break;
		}
		else{
			num_try++;
			if(num_try == 99){
				printf("Error: max retries reached, could not bind server\n");
				exit(1);
			}
		}		
	}
	if (listen(create_socket, 1000) < 0){
		perror("Server: listen");
		exit(1);
	}
	int slot = 0;
	while (1){
		if ((clients[slot] = accept(create_socket, (struct sockaddr *) &address, &addrlen)) > 0){
				if (fork() == 0){
					respond(slot);
					slot++;
					exit(0);
				}
		}
		else{
			perror("Server: accept");
			exit(1);	
		}
	}
}

void configureServer(char *path){
	int fd, bytes_read;
	char *data_to_send;
	char *file_type;
	data_to_send = malloc(BUFFER_SIZE);

	int i = 0;
	for(i; i<MAX_SUPPORTED_TYPES; i++){
		supported_types[i] = "";
	}

	int n = 0;
	if((fd=open(path, O_RDONLY))!=-1){
		 while((bytes_read=read(fd, data_to_send, BUFFER_SIZE))>0){
		 	char *token = strtok(data_to_send, " \t\n");
		 	while(token != NULL){
		 		if(strcmp(token, "Listen") == 0){
		 			token = strtok(NULL, " \t\n");
		 			PORT = token;
		 		}
		 		else if(token[0] == '.'){
		 			char new_token[strlen(token)];
		 			char extension[strlen(token) - 1];
		 			strcpy(new_token, token);
		 			int i = 1;
		 			int j = 0;
		 			for(i;i<strlen(token);i++){
		 				extension[j] = new_token[i];
		 				j++;
		 			}
		 			file_type = extension;
		 			supported_types[n] = file_type;
		 			n++;
		 		}
		 		token = strtok(NULL, " \t\n");
		 	}
		 }
	}
	else{
		printf("Configuration file not found\n");
		exit(1);
	}

}

int main(int argc, char* argv[]){
	if(argc > 1){
		char *conf_file = argv[1];
		configureServer(conf_file);
		initializeServer(PORT);
		return 0;	
	}
	else{
		printf("usage: ./main <CONFIGURATION_FILE>\n");
	}
}
