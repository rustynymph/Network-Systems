#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_CONNECTIONS 1000
#define BUFFER_SIZE 1024
#define MAX_RETRIES 100

char *directory;

typedef struct{
    char *num;
    char *name;
    char *buffer;
} file_partition;

void writeToDisk(file_partition file){
	printf("\nWRITING TO DISK:\n");
	printf("================\n");
	printf("File piece number: %s\n", file.num);
	printf("File name: %s\n", file.name);
	printf("File contents: %s\n", file.buffer);

	char* new_file_name;
	new_file_name = malloc(strlen(directory) + 1 + strlen(file.name) + 2);
	sprintf(new_file_name, "%s/%s.%s", directory, file.name, file.num);

	FILE *fp;
	char ch;
	fp=fopen(new_file_name,"w");
	//fwrite(file.buffer, 1, sizeof(file.buffer), fp);
	fprintf(fp, "%s", file.buffer);
	fclose(fp);
}

void handleGetRequest(int socket){
	printf("\nHandling GET request...\n");
	int fd;
	char *mesg;
	mesg = malloc(256);
	int filename_bytes;
	int download;
	char *filename;
	int pieceThere[4] = {0, 0, 0, 0};

	if (recv(socket, &filename_bytes, 4, 0) >= 1){
		printf("Received filenamebytes: %d\n", filename_bytes);
		filename = malloc(filename_bytes);
	}
	if (recv(socket, mesg, filename_bytes, 0) >= 1){
		printf("Received filename: %s\n", mesg);
		filename = mesg;
	}

	char *filename1;
	filename1 = malloc(strlen(filename) + 2);
	sprintf(filename1, "%s/%s.1", directory, filename);
	char *filename2;
	filename2 = malloc(strlen(filename) + 2);
	sprintf(filename2, "%s/%s.2", directory, filename);
	char *filename3;
	filename3 = malloc(strlen(filename) + 2);
	sprintf(filename3, "%s/%s.3", directory, filename);
	char *filename4;
	filename4 = malloc(strlen(filename) + 2);
	sprintf(filename4, "%s/%s.4", directory, filename);

	if((fd=open(filename1, O_RDONLY))!=-1){
		pieceThere[0] = 1;
	}
	if((fd=open(filename2, O_RDONLY))!=-1){
		pieceThere[1] = 1;
	}
	if((fd=open(filename3, O_RDONLY))!=-1){
		pieceThere[2] = 1;
	}
	if((fd=open(filename4, O_RDONLY))!=-1){
		pieceThere[3] = 1;
	}

	send(socket, &pieceThere[0], 4, 0);					
	send(socket, &pieceThere[1], 4, 0);					
	send(socket, &pieceThere[2], 4, 0);	
	send(socket, &pieceThere[3], 4, 0);	

	int pieceNum1;
	int pieceNum2;
	char *filename_d;
	char *filename_d2;
	filename_d = malloc(strlen(filename) + 2);
	filename_d2 = malloc(strlen(filename) + 2);
	recv(socket, &download, 4, 0);

	if (download == 1){
		recv(socket, &pieceNum1, 4, 0);
		if(pieceNum1 == 1){ filename_d =  filename1;}
		else if(pieceNum1 == 2){ filename_d =  filename2;}
		else if(pieceNum1 == 3){ filename_d =  filename3;}
		else if(pieceNum1 == 4){ filename_d =  filename4;}

		printf("\nREADING FROM DISK:\n");
		printf("================\n");

		FILE *f = fopen(filename_d, "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);
		char *buffer_1 = malloc(fsize + 1);
		fread(buffer_1, fsize, 1, f);
		fclose(f);		

		printf("File size of piece %d: %ld\n", pieceNum1, fsize);
		printf("File contents of piece %d: %s\n", pieceNum1, buffer_1);
		send(socket, &fsize, 8, 0);
		send(socket, buffer_1, fsize, 0);


	}
	else if(download == 2){
		recv(socket, &pieceNum1, 4, 0);
		recv(socket, &pieceNum2, 4, 0);

		if(pieceNum1 == 1){ filename_d =  filename1;}
		else if(pieceNum1 == 2){ filename_d =  filename2;}
		else if(pieceNum1 == 3){ filename_d =  filename3;}
		else if(pieceNum1 == 4){ filename_d =  filename4;}

		if(pieceNum2 == 1){ filename_d2 =  filename1;}
		else if(pieceNum2 == 2){ filename_d2 =  filename2;}
		else if(pieceNum2 == 3){ filename_d2 =  filename3;}
		else if(pieceNum2 == 4){ filename_d2 =  filename4;}		

		printf("\nREADING FROM DISK:\n");
		printf("================\n");

		FILE *f = fopen(filename_d, "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);
		char *buffer_1 = malloc(fsize + 1);
		fread(buffer_1, fsize, 1, f);
		fclose(f);		

		FILE *f2 = fopen(filename_d2, "rb");
		fseek(f2, 0, SEEK_END);
		long fsize2 = ftell(f2);
		fseek(f2, 0, SEEK_SET);
		char *buffer_2 = malloc(fsize2 + 1);
		fread(buffer_2, fsize2, 1, f);
		fclose(f2);				

		printf("File size of piece %d: %ld\n", pieceNum1, fsize);
		printf("File contents of piece %d: %s\n", pieceNum1, buffer_1);
		printf("File size of piece %d: %ld\n", pieceNum2, fsize2);
		printf("File contents of piece %d: %s\n", pieceNum2, buffer_2);

		send(socket, &fsize, 8, 0);
		send(socket, &fsize2, 8, 0);	

		send(socket, buffer_1, fsize, 0);
		send(socket, buffer_2, fsize2, 0);
	}				
}

void handleListRequest(int socket){
}

void handlePutRequest(int socket){
	printf("\nHandling PUT request...\n");
	char *mesg;
	mesg = malloc(256);
	char *file_piece1;
	file_piece1 = malloc(1);
	char *file_piece2;
	file_piece2 = malloc(1);
	int filename_bytes;
	int filepart1_bytes;
	int filepart2_bytes;
	char *buffer1;
	char *buffer2;
	char *filename;
	file_partition file1;
	file_partition file2;

	if (recv(socket, &filename_bytes, 4, 0) >= 1){
		printf("Received filenamebytes: %d\n", filename_bytes);
		filename = malloc(filename_bytes);
	}
	if (recv(socket, mesg, filename_bytes, 0) >= 1){
		printf("Received filename: %s\n", mesg);
		filename = mesg;
	}			
	if (recv(socket, file_piece1, 1, 0) >= 1){
		printf("Received file piece part num: %s\n", file_piece1);
		file1.num = malloc(1);
		file1.num = file_piece1;
	}
	if (recv(socket, file_piece2, 1, 0) >= 1){
		printf("Received file piece part num: %s\n", file_piece2);
		file2.num = malloc(1);
		file2.num = file_piece2;
	}
	if (recv(socket, &filepart1_bytes, 4, 0) >= 1){
		printf("Received file part 1 bytes: %d\n", filepart1_bytes);
		buffer1 = malloc(filepart1_bytes);
	}	
	if (recv(socket, &filepart2_bytes, 4, 0) >= 1){
		printf("Received file part 2 bytes: %d\n", filepart2_bytes);
		buffer2 = malloc(filepart2_bytes);
	}
	if (recv(socket, buffer1, filepart1_bytes, 0) >= 1){
		
		file1.name = malloc(filename_bytes);
		file1.name = filename;
		file1.buffer = malloc(filepart1_bytes);
		file1.buffer = buffer1;
		writeToDisk(file1);
	}	
	if (recv(socket, buffer2, filepart2_bytes, 0) >= 1){
		file2.name = malloc(filename_bytes);
		file2.name = filename;
		file2.buffer = malloc(filepart2_bytes);
		file2.buffer = buffer2;
		writeToDisk(file2);		
	}			
}

void receieveMessages(int socket){
	char *mesg;
	mesg = malloc(1);
	while(1){
		if (recv(socket, mesg, 1, 0) >= 1){
			if(strcmp(mesg, "0") == 0){
				handlePutRequest(socket);
			}
			else if(strcmp(mesg, "1") == 0){
				handleGetRequest(socket);
			}
			else if(strcmp(mesg, "2") == 0){
				handleListRequest(socket);
			}							
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
			printf("Socket bound on port %d\n", port);
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
	int connected_client;
	while (1){
		if ((connected_client = accept(create_socket, (struct sockaddr *) &address, &addrlen)) > 0){
			printf("Connected client!\n");
			if (fork() == 0){
				receieveMessages(connected_client);
				exit(0);
			}
		}
		else{
			perror("Server: accept");
			exit(1);	
		}
	}	

	//shutdown(create_socket, SHUT_RDWR);
	//close(create_socket);
}


int main(int argc, char* argv[]){
	if(argc > 2){
		directory = argv[1];
		int port = atoi(argv[2]);
		initializeServer(port);
		return 0;	
	}
	else{
		printf("USAGE: ./dfs <SERVER DIRECTORY> <PORT NUMBER>\n");
	}

	return 0;
}
