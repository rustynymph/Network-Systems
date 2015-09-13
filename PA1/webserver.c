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

void sendText(int socket, char *filename){
	//STUB
}

void sendImage(int socket, char *filename){
	//STUB
}

void sendHTMLPage(int socket, char *filename){

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

			write(socket, "HTTP/1.0 200 OK\n", 16);
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
		close(socket);
	}

}

void handleGETRequest(char *request){
	//STUB
}

void *listenForRequests(void *socket){
	char *buffer;
	buffer = malloc(BUFFER_SIZE);
	const char delims[2] = " ";

		recv(*(int*)socket, buffer, BUFFER_SIZE, 0);
		char buffer_cpy[BUFFER_SIZE];
		strcpy(buffer_cpy, buffer);
		//printf("%s\n", buffer_cpy);
		
		char *token = strtok(buffer_cpy, delims);
		while(token != NULL){
			printf("%s\n", token);
			token = strtok(NULL, delims);
			
		
	}
}

void initializeServer(int port){

	int create_socket, new_socket;
	socklen_t addrlen;
	//char *buffer = malloc(BUFFER_SIZE);
	struct sockaddr_in address;

	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){
		printf("Socket created successfully\n");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	int num_try = 0;
	while(num_try < MAX_RETRIES){
		if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){
			printf("Binding socket\n");
			break;
		}
		else{
			num_try++;
			if(num_try == 99){
				printf("Error: max retries reached, could not bind server");
				break;
			}
		}		
	}


		if (listen(create_socket, 10) < 0){
			perror("Server: listen");
			exit(1);
		} //this used to be inside the while loop

	//while (1){


		if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) > 0){
			printf("The client is connected...\n");
			sendHTMLPage(new_socket, "www/index.html");
		}

		else{
			perror("Server: accept");
			exit(1);	
		}


/*
		if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0){
			perror("Server: accept");
			exit(1);
		}


				if (new_socket > 0){
			printf("The client is connected...\n");
			sendHTMLPage(new_socket, "www/index.html");
			//pthread_t listener_thread;
			//pthread_create(&listener_thread, NULL, listenForRequests, &new_socket);

		}*/
		//recv(new_socket, buffer, BUFFER_SIZE, 0);
		//printf("%s\n", buffer);

		



		//spawn thread for ew new client
	//}

	close(create_socket);
}

int main(){
	initializeServer(8097);
	return 0;
}