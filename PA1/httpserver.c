#include "httpserver.h"
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

int BUFFER_SIZE = 4096;
int MAX_RETRIES = 100;

struct arg_struct {
    int arg1;
    char *arg2;
};

const char *get_filename_ext(const char *filename) {
	printf("In getFilenameExt\n");
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    printf("donezo\n");
    return dot + 1;
}

void *sendText(void *arguments){
	struct arg_struct *args = arguments;
	int socket = args->arg1;
	const char *filename = args->arg2;
	if(filename[0] == '/')	filename++;

	FILE *f = fopen(filename, "r");
	if (f){
		char *buffer = 0;
		long length;

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(BUFFER_SIZE);
		if (buffer){

			char *content_length_string = "Content-length: %d\n", length;
			int content_length_string_size = sizeof(content_length_string);

			write(socket, "HTTP/1.1 200 OK\n", 16);
			write(socket, "Content-Type: text\n", 24);
			write(socket, content_length_string, content_length_string_size);
			write(socket, "Connection: keep-alive\n\n", 24);

			size_t bytes_read = 0;
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0){
				write(socket, buffer, (int)bytes_read);
			}
			fclose(f);
			free(buffer);			
		}
	}
}


void *sendJPG(void *arguments){
	struct arg_struct *args = arguments;
	int socket = args->arg1;
	const char *filename = args->arg2;
	if(filename[0] == '/')	filename++;

	FILE *f = fopen(filename, "r");
	if (f){
		char *buffer = 0;
		long length;

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(BUFFER_SIZE);
		if (buffer){

			char *content_length_string = "Content-length: %d\n", length;
			int content_length_string_size = sizeof(content_length_string);

			write(socket, "HTTP/1.1 200 OK\n", 16);
			write(socket, "Content-Type: image/jpg\n", 24);
			write(socket, content_length_string, content_length_string_size);
			write(socket, "Connection: keep-alive\n\n", 24);

			size_t bytes_read = 0;
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0){
				write(socket, buffer, (int)bytes_read);
			}
			fclose(f);
			free(buffer);			
		}
	}
}

void *sendPNG(void *arguments){
	struct arg_struct *args = arguments;
	int socket = args->arg1;
	const char *filename = args->arg2;
	if(filename[0] == '/')	filename++;

	FILE *f = fopen(filename, "r");
	if (f){
		char *buffer = 0;
		long length;

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(BUFFER_SIZE);
		if (buffer){

			char *content_length_string = "Content-length: %d\n", length;
			int content_length_string_size = sizeof(content_length_string);

			write(socket, "HTTP/1.1 200 OK\n", 16);
			write(socket, "Content-Type: image/png\n", 24);
			write(socket, content_length_string, content_length_string_size);
			write(socket, "Connection: keep-alive\n\n", 24);

			size_t bytes_read = 0;
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0){
				write(socket, buffer, (int)bytes_read);
			}
			fclose(f);
			free(buffer);			
		}
	}
}

void *sendGIF(void *arguments){
	struct arg_struct *args = arguments;
	int socket = args->arg1;
	const char *filename = args->arg2;
	if(filename[0] == '/')	filename++;

	FILE *f = fopen(filename, "r");
	if (f){
		char *buffer = 0;
		long length;

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(BUFFER_SIZE);
		if (buffer){

			char *content_length_string = "Content-length: %d\n", length;
			int content_length_string_size = sizeof(content_length_string);

			write(socket, "HTTP/1.1 200 OK\n", 16);
			write(socket, "Content-Type: image/gif\n", 24);
			write(socket, content_length_string, content_length_string_size);
			write(socket, "Connection: keep-alive\n\n", 24);

			size_t bytes_read = 0;
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0){
				write(socket, buffer, (int)bytes_read);
			}
			fclose(f);
			free(buffer);			
		}
	}
}

void *sendJavaScript(void *arguments){
	struct arg_struct *args = arguments;
	int socket = args->arg1;
	char *filename = args->arg2;
	if(filename[0] == '/')	filename++;
	FILE *f = fopen(filename, "r");
	
	if (f){
		char *buffer = 0;
		long length;

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(BUFFER_SIZE);
		if (buffer){

			char *content_length_string = "Content-length: %d\n", length;
			int content_length_string_size = sizeof(content_length_string);

			write(socket, "HTTP/1.1 200 OK\n", 16);
			write(socket, "Content-Type: application/javascript\n", 37);
			write(socket, content_length_string, content_length_string_size);
			write(socket, "Connection: keep-alive\n\n", 24);

			size_t bytes_read = 0;
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0){
				write(socket, buffer, (int)bytes_read);
			}

			fclose(f);
			free(buffer);			
		}
	}
}

void *sendCSS(void *arguments){

	struct arg_struct *args = arguments;
	int socket = args->arg1;
	char *filename = args->arg2;
	if(filename[0] == '/')	filename++;

	FILE *f = fopen(filename, "rb");
	if (f){

		char *buffer = 0;
		long length;

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(BUFFER_SIZE);
		if (buffer){

			char *content_length_string = "Content-length: %d\n", length;
			int content_length_string_size = sizeof(content_length_string);

			write(socket, "HTTP/1.1 200 OK\n", 16);
			write(socket, "Content-Type: text/css\n", 24);
			write(socket, content_length_string, content_length_string_size);
			write(socket, "Connection: keep-alive\n\n", 24);

			size_t bytes_read = 0;
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0){
				write(socket, buffer, (int)bytes_read);
			}

			fclose(f);
			
			free(buffer);			
		}
	}
		

	else{
		printf("Returning 404\n");
		char *not_found_string = "HTTP/1.1 404 Not Found: %s\n", filename;

		write(socket, "HTTP/1.1 404 Not Found\n", 40);
		write(socket, "Connection: keep-alive\n\n", 24);
		write(socket, "<html><body><H1>404 Error: Page not found</H1></body></html>", 60);
		//close(socket);
	}

}

void sendHTMLPage(int socket, char *filename){

	printf("In sendHTMLPage\n");
	if(filename[0] == '/')	filename++;
	FILE *f = fopen(filename, "rb");
	if (f){
		printf("File found\n");

		char *buffer = 0;
		long length;

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(BUFFER_SIZE);
		if (buffer){

			char *content_length_string = "Content-length: %d\n", length;
			int content_length_string_size = sizeof(content_length_string);

			write(socket, "HTTP/1.1 200 OK\n", 16);
			write(socket, "Content-Type: text/html\n", 24);
			write(socket, content_length_string, content_length_string_size);
			write(socket, "Connection: keep-alive\n\n", 24);

			size_t bytes_read = 0;
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0){
				write(socket, buffer, (int)bytes_read);
			}

			fclose(f);
			
			close(socket);	
			free(buffer);			
		}
	}
		

	else{
		printf("Returning 404\n");
		char *not_found_string = "HTTP/1.1 404 Not Found: %s\n", filename;

		write(socket, "HTTP/1.1 404 Not Found\n", 40);
		write(socket, "Connection: keep-alive\n\n", 24);
		write(socket, "<html><body><H1>404 Error: Page not found</H1></body></html>", 60);
		//close(socket);
	}

}

void parseHTTPrequest(char *request, int socket){
	printf("%s from socket %d\n", request, socket);

	char buffer_cpy[BUFFER_SIZE];
	strcpy(buffer_cpy, request);

	char *token = strtok(buffer_cpy, " \n");
	while(token != NULL){
		if (strcmp(token, "GET") == 0){

			token = strtok(NULL, " \n");

			size_t ln = strlen(token) - 1;
			if (token[ln] == '\n'){
				token[ln] = '\0';
			}

			if (strcmp(token, "/") == 0){
				sendHTMLPage(socket, "index.html");
			}
			else{
				const char *extension = get_filename_ext(token);
	  				  
				//pthread_t send_response_thread;

				struct arg_struct args;
				args.arg1 = socket;
				args.arg2 = token;

				if (strcmp(extension, "gif") == 0){
					sendGIF(&args);
					//pthread_create(&send_response_thread, NULL, sendGIF, &args);
				}
				else if(strcmp(extension, "js") == 0){
					sendJavaScript(&args);
					//pthread_create(&send_response_thread, NULL, sendJavaScript, &args);
				}
				else if(strcmp(extension, "png") == 0){
					sendPNG(&args);
				}
				else if(strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0){
					sendJPG(&args);
				}
				else if(strcmp(extension, "css") == 0){
					sendCSS(&args);
				}
				else if(strcmp(extension, "html") == 0 || strcmp(extension, "htm") == 0){
					sendHTMLPage(socket, token);
				}
				else if(strcmp(extension, "txt") == 0){
					sendText(&args);
				}
				//pthread_join(send_response_thread, NULL);
				break;
			}

		}
		
		else{
			token = strtok(NULL, " \n");
		}
	}
}

void handleGETRequest(char *filename, int socket){
	FILE *f = fopen(filename, "rb");
	if (f){
	}
	else{
		printf("Returning 404\n");
		char *not_found_string = "HTTP/1.1 404 Not Found: %s\n", filename;

		write(socket, "HTTP/1.1 404 Not Found\n", 40);
		write(socket, "Connection: keep-alive\n\n", 24);
		write(socket, "<html><body><H1>404 Error: Page not found</H1></body></html>", 60);
		close(socket);		
	}
}

void *listenForRequests(void *socket){
	char *buffer;
	buffer = malloc(BUFFER_SIZE);

	while(1){
		recv(*(int*)socket, buffer, BUFFER_SIZE, 0);
		if ((strcmp(buffer, "\0") != 0) && (strcmp(buffer, "\n\0") != 0)){
			printf("hi\n");
			parseHTTPrequest(buffer, *(int*)socket);
		}
	}
}

void initializeServer(int port){

	int create_socket, new_socket;
	socklen_t addrlen;
	char *buffer = malloc(BUFFER_SIZE);
	struct sockaddr_in address;

	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){
		printf("Socket created successfully\n");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

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


	if (listen(create_socket, 10) < 0){
		perror("Server: listen");
		exit(1);
	} //this used to be inside the while loop


	struct client_list {
		int val;
		struct client_list * next;
	};

	typedef struct client_list item;

	item * curr, * head;
	int i;

	head = NULL;

	int test = 0;
	while (1){
		int already_connected = 0; //0 means not connected, 1 is connected
		curr = head;
		if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) > 0){
			while(curr){
				if(new_socket == curr->val){
					already_connected = 1;
					break;
				}
				curr = curr->next;
			}
			

			if (already_connected == 0){
				curr = (item *)malloc(sizeof(item));
				curr->val = new_socket;
				curr->next = head;
				head = curr;


				printf("The client is connected...\n");
				printf("%d\n", new_socket);
				
				//sendHTMLPage(new_socket, "index.html");
				pthread_t listener_thread;
				pthread_create(&listener_thread, NULL, listenForRequests, &new_socket);
				//listenForRequests(&new_socket);

				//pthread_join(listener_thread, NULL);

			}
			else{
				printf("testing...");
			}

		}

		else{
			perror("Server: accept");
			exit(1);	
		}
	}

}


