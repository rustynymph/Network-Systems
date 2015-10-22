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
#include<openssl/md5.h>

#define MAX_RETRIES 100
#define BUFFERSIZE 256
#define MAX_USERS 20
int servers[4];
int server1;
int server2;
int server3;
int server4;
int usercount = 0;

typedef struct{
	int piece1Sock;
	int piece2Sock;
	int piece3Sock;
	int piece4Sock;
} file_piece_locations;

typedef struct{
	char usernames[MAX_USERS][16];
	char passwords[MAX_USERS][32];
}config_information;

config_information info;

typedef struct{
	int server;
	int filenum;
	char *buffer;
}download_info;

int authenticate(){
	int user_index = -1;
	int validUsername = 0;
	int validPassword = 0;
	char *buffer;
	buffer = malloc(BUFFERSIZE);	

	int i = 0;

	printf("\nUsername: ");
    if(fgets(buffer, BUFFERSIZE , stdin) != NULL){
    	strtok(buffer, "\n");
    	for(i; i <= usercount; i++){ 
        	if(strcmp(buffer, info.usernames[i]) == 0){
        		validUsername = 1;
        		user_index = i;
        	}		
    	}
    }
	printf("Password: ");
    if(fgets(buffer, BUFFERSIZE , stdin) != NULL){
    	strtok(buffer, "\n");
    	if(validUsername == 1){
    	    if(strcmp(buffer, info.passwords[user_index]) == 0){
    			validPassword = 1;
    		}	
    	}
    	else{
    		validPassword = 0;
    	}

    }

    return validUsername & validPassword;    
}

int isFileComplete(file_piece_locations locations){
	if((locations.piece1Sock != -1) && (locations.piece2Sock != -1) && (locations.piece3Sock != -1) && (locations.piece4Sock != -1)){ return 1; }
	return 0;
}

file_piece_locations mapFilePieceToServer(int server1Pieces[4], int server2Pieces[4], int server3Pieces[4], int server4Pieces[4]){
	file_piece_locations locations;

	locations.piece1Sock = -1;
	locations.piece2Sock = -1;
	locations.piece3Sock = -1;
	locations.piece4Sock = -1;

	if(server1Pieces[0] == 1){ locations.piece1Sock = server1; }	
	if(server1Pieces[1] == 1){ locations.piece2Sock = server1; }	
	if(server1Pieces[2] == 1){ locations.piece3Sock = server1; }	
	if(server1Pieces[3] == 1){ locations.piece4Sock = server1; }

	if(server2Pieces[0] == 1){ locations.piece1Sock = server2; }	
	if(server2Pieces[1] == 1){ locations.piece2Sock = server2; }	
	if(server2Pieces[2] == 1){ locations.piece3Sock = server2; }	
	if(server2Pieces[3] == 1){ locations.piece4Sock = server2; }	

	if(server3Pieces[0] == 1){ locations.piece1Sock = server3; }	
	if(server3Pieces[1] == 1){ locations.piece2Sock = server3; }	
	if(server3Pieces[2] == 1){ locations.piece3Sock = server3; }	
	if(server3Pieces[3] == 1){ locations.piece4Sock = server3; }	

	if(server4Pieces[0] == 1){ locations.piece1Sock = server4; }	
	if(server4Pieces[1] == 1){ locations.piece2Sock = server4; }	
	if(server4Pieces[2] == 1){ locations.piece3Sock = server4; }	
	if(server4Pieces[3] == 1){ locations.piece4Sock = server4; }					

	return locations;
}

void downloadFiles(char *filename, file_piece_locations locations){
	int downloadFromServer1, downloadFromServer2, downloadFromServer3, downloadFromServer4;
	downloadFromServer1 = downloadFromServer2 = downloadFromServer3 = downloadFromServer4 = 0;
	download_info piece1;
	piece1.filenum = 1;
	download_info piece2;
	piece2.filenum = 2;
	download_info piece3;
	piece3.filenum = 3;
	download_info piece4;
	piece4.filenum = 4;

	if (locations.piece1Sock == server1){ piece1.server = server1; downloadFromServer1++;}
	else if (locations.piece1Sock == server2){ piece1.server = server2; downloadFromServer2++;}
	else if (locations.piece1Sock == server3){ piece1.server = server3; downloadFromServer3++;}
	else if (locations.piece1Sock == server4){ piece1.server = server4; downloadFromServer4++;}

	if (locations.piece2Sock == server1){ piece2.server = server1; downloadFromServer1++;}
	else if (locations.piece2Sock == server2){ piece2.server = server2; downloadFromServer2++;}
	else if (locations.piece2Sock == server3){ piece2.server = server3; downloadFromServer3++;}
	else if (locations.piece2Sock == server4){ piece2.server = server4; downloadFromServer4++;}

	if (locations.piece3Sock == server1){ piece3.server = server1; downloadFromServer1++;}
	else if (locations.piece3Sock == server2){ piece3.server = server2; downloadFromServer2++;}
	else if (locations.piece3Sock == server3){ piece3.server = server3; downloadFromServer3++;}
	else if (locations.piece3Sock == server4){ piece3.server = server4; downloadFromServer4++;}

	if (locations.piece4Sock == server1){ piece4.server = server1; downloadFromServer1++;}
	else if (locations.piece4Sock == server2){ piece4.server = server2; downloadFromServer2++;}
	else if (locations.piece4Sock == server3){ piece4.server = server3; downloadFromServer3++;}
	else if (locations.piece4Sock == server4){ piece4.server = server4; downloadFromServer4++;}

	send(server1, &downloadFromServer1, 4, 0);
	send(server2, &downloadFromServer2, 4, 0);
	send(server3, &downloadFromServer3, 4, 0);
	send(server4, &downloadFromServer4, 4, 0);

	send(locations.piece1Sock, &piece1.filenum, 4, 0);
	send(locations.piece2Sock, &piece2.filenum, 4, 0);
	send(locations.piece3Sock, &piece3.filenum, 4, 0);
	send(locations.piece4Sock, &piece4.filenum, 4, 0);

	int piece1BufferSize, piece2BufferSize, piece3BufferSize, piece4BufferSize;
	recv(locations.piece1Sock, &piece1BufferSize, 8, 0);
	recv(locations.piece2Sock, &piece2BufferSize, 8, 0);
	recv(locations.piece3Sock, &piece3BufferSize, 8, 0);
	recv(locations.piece4Sock, &piece4BufferSize, 8, 0);

	piece1.buffer = malloc(piece1BufferSize);
	piece2.buffer = malloc(piece2BufferSize);
	piece3.buffer = malloc(piece3BufferSize);
	piece4.buffer = malloc(piece4BufferSize);

	recv(locations.piece1Sock, piece1.buffer, piece1BufferSize, 0);
	printf("Buffer from piece 1: %s\n", piece1.buffer);
	recv(locations.piece2Sock, piece2.buffer, piece2BufferSize, 0);
	printf("Buffer from piece 2: %s\n", piece2.buffer);
	recv(locations.piece3Sock, piece3.buffer, piece3BufferSize, 0);
	printf("Buffer from piece 3: %s\n", piece3.buffer);
	recv(locations.piece4Sock, piece4.buffer, piece4BufferSize, 0);
	printf("Buffer from piece 4: %s\n", piece4.buffer);

	FILE *fp;
	fp=fopen(filename,"w");
	fprintf(fp, "%s", piece1.buffer);
	fclose(fp);

	fp=fopen(filename,"a");
	fprintf(fp, "%s", piece2.buffer);
	fclose(fp);

	fp=fopen(filename,"a");
	fprintf(fp, "%s", piece3.buffer);
	fclose(fp);

	fp=fopen(filename,"a");
	fprintf(fp, "%s", piece4.buffer);
	fclose(fp);		
}

void retrieveFileFromServers(char *filename){
	int filename_length = strlen(filename);
	int piece;
	int pieceThere1[4];
	int pieceThere2[4];
	int pieceThere3[4];
	int pieceThere4[4];
	int piece1=0;
	int piece2=0;
	int piece3=0;
	int piece4=0;
	int piece1_sock=0;
	int piece2_sock=0;
	int piece3_sock=0;
	int piece4_sock=0;	

	//printf("sending first header\n");
	send(server1, "1", 1, 0);
	send(server2, "1", 1, 0);
	send(server3, "1", 1, 0);
	send(server4, "1", 1, 0);

	//printf("sending file length\n");
	send(server1, &filename_length, sizeof(filename_length), 0);
	send(server2, &filename_length, sizeof(filename_length), 0);
	send(server3, &filename_length, sizeof(filename_length), 0);
	send(server4, &filename_length, sizeof(filename_length), 0);

	//printf("sending filename\n");
	send(server1, filename, filename_length, 0);
	send(server2, filename, filename_length, 0);
	send(server3, filename, filename_length, 0);
	send(server4, filename, filename_length, 0);

	recv(server1, &piece, 4, 0);
	pieceThere1[0] = piece;
	recv(server1, &piece, 4, 0);
	pieceThere1[1] = piece;
	recv(server1, &piece, 4, 0);
	pieceThere1[2] = piece;
	recv(server1, &piece, 4, 0);
	pieceThere1[3] = piece;

	recv(server2, &piece, 4, 0);
	pieceThere2[0] = piece;
	recv(server2, &piece, 4, 0);
	pieceThere2[1] = piece;
	recv(server2, &piece, 4, 0);
	pieceThere2[2] = piece;
	recv(server2, &piece, 4, 0);
	pieceThere2[3] = piece;

	recv(server3, &piece, 4, 0);
	pieceThere3[0] = piece;
	recv(server3, &piece, 4, 0);
	pieceThere3[1] = piece;
	recv(server3, &piece, 4, 0);
	pieceThere3[2] = piece;
	recv(server3, &piece, 4, 0);
	pieceThere3[3] = piece;

	recv(server4, &piece, 4, 0);
	pieceThere4[0] = piece;
	recv(server4, &piece, 4, 0);
	pieceThere4[1] = piece;
	recv(server4, &piece, 4, 0);
	pieceThere4[2] = piece;
	recv(server4, &piece, 4, 0);
	pieceThere4[3] = piece;

	file_piece_locations locations = mapFilePieceToServer(pieceThere1, pieceThere2, pieceThere3, pieceThere4);

	int complete;
	if ((complete = isFileComplete(locations)) > 0){
		printf("%s is complete\n", filename);
		downloadFiles(filename, locations);
	}
	else{
		printf("%s is incomplete\n", filename);
		send(server1, 0, 4, 0);
		send(server2, 0, 4, 0);
		send(server3, 0, 4, 0);
		send(server4, 0, 4, 0);
	}
}


void splitFileOntoServers(int key, char *filename){
	int fd;
	int bytes_read;
	int file_size;
	if((fd=open(filename, O_RDONLY))!=-1){

    	int file_size = lseek(fd, 0, SEEK_END);
    	lseek(fd, 0, SEEK_SET);

		int file_size_piece = file_size / 4;
		char p1_buffer[file_size_piece];
		char p2_buffer[file_size_piece];
		char p3_buffer[file_size_piece];
		int difference = file_size - 3*(file_size_piece);
		char p4_buffer[difference];
		int size_p1_buffer = read(fd, p1_buffer, file_size_piece);
		int size_p2_buffer = read(fd, p2_buffer, file_size_piece);
		int size_p3_buffer = read(fd, p3_buffer, file_size_piece);
		int size_p4_buffer = read(fd, p4_buffer, difference);

		int filename_length = strlen(filename);

		//printf("sending first header\n");
		send(server1, "0", 1, 0);
		send(server2, "0", 1, 0);
		send(server3, "0", 1, 0);
		send(server4, "0", 1, 0);

		//printf("sending file length\n");
		send(server1, &filename_length, sizeof(filename_length), 0);
		send(server2, &filename_length, sizeof(filename_length), 0);
		send(server3, &filename_length, sizeof(filename_length), 0);
		send(server4, &filename_length, sizeof(filename_length), 0);

		//printf("sending filename\n");
		send(server1, filename, filename_length, 0);
		send(server2, filename, filename_length, 0);
		send(server3, filename, filename_length, 0);
		send(server4, filename, filename_length, 0);					

		if(key == 0){
			//send pieces 1 and 2 to server 1
			send(server1, "1", 1, 0);
			send(server1, "2", 1, 0);
			send(server1, &size_p1_buffer, 4, 0);
			send(server1, &size_p2_buffer, 4, 0);
			send(server1, p1_buffer, size_p1_buffer, 0);
			send(server1, p2_buffer, size_p2_buffer, 0);

			//send pieces 2 and 3 to server 2
			send(server2, "2", 1, 0);
			send(server2, "3", 1, 0);	
			send(server2, &size_p2_buffer, 4, 0);
			send(server2, &size_p3_buffer, 4, 0);					
			send(server2, p2_buffer, size_p2_buffer, 0);
			send(server2, p3_buffer, size_p3_buffer, 0);

			//send pieces 3 and 4 to server 3
			send(server3, "3", 1, 0);
			send(server3, "4", 1, 0);
			send(server3, &size_p3_buffer, 4, 0);
			send(server3, &size_p4_buffer, 4, 0);						
			send(server3, p3_buffer, size_p3_buffer, 0);
			send(server3, p4_buffer, size_p4_buffer, 0);

			//send pieces 4 and 1 to server 4
			send(server4, "4", 4, 0);
			send(server4, "1", 4, 0);
			send(server4, &size_p4_buffer, 4, 0);
			send(server4, &size_p1_buffer, 4, 0);							
			send(server4, p4_buffer, size_p4_buffer, 0);
			send(server4, p1_buffer, size_p1_buffer, 0);									
		}						

		else if(key == 1){
			//send pieces 4 and 1 to server 1
			send(server1, "4", 1, 0);
			send(server1, "1", 1, 0);
			send(server1, &size_p4_buffer, 4, 0);
			send(server1, &size_p1_buffer, 4, 0);							
			send(server1, p4_buffer, size_p4_buffer, 0);
			send(server1, p1_buffer, size_p1_buffer, 0);	

			//send pieces 1 and 2 to server 2
			send(server2, "1", 1, 0);
			send(server2, "2", 1, 0);
			send(server2, &size_p1_buffer, 4, 0);
			send(server2, &size_p2_buffer, 4, 0);							
			send(server2, p1_buffer, size_p1_buffer, 0);
			send(server2, p2_buffer, size_p2_buffer, 0);
			
			//send pieces 2 and 3 to server 3
			send(server3, "2", 1, 0);
			send(server3, "3", 1, 0);
			send(server3, &size_p2_buffer, 4, 0);
			send(server3, &size_p3_buffer, 4, 0);							
			send(server3, p2_buffer, size_p2_buffer, 0);
			send(server3, p3_buffer, size_p3_buffer, 0);
			
			//send pieces 3 and 4 to server 4
			send(server4, "3", 1, 0);
			send(server4, "4", 1, 0);
			send(server4, &size_p3_buffer, 4, 0);
			send(server4, &size_p4_buffer, 4, 0);							
			send(server4, p3_buffer, size_p3_buffer, 0);
			send(server4, p4_buffer, size_p4_buffer, 0);
		}	

		else if(key == 2){
			//send pieces 3 and 4 to server 1
			send(server1, "3", 1, 0);
			send(server1, "4", 1, 0);
			send(server1, &size_p3_buffer, 4, 0);
			send(server1, &size_p4_buffer, 4, 0);							
			send(server1, p3_buffer, size_p3_buffer, 0);
			send(server1, p4_buffer, size_p4_buffer, 0);

			//send pieces 4 and 1 to server 2
			send(server2, "4", 1, 0);
			send(server2, "1", 1, 0);
			send(server2, &size_p4_buffer, 4, 0);
			send(server2, &size_p1_buffer, 4, 0);							
			send(server2, p4_buffer, size_p4_buffer, 0);
			send(server2, p1_buffer, size_p1_buffer, 0);

			//send pieces 1 and 2 to server 3
			send(server3, "1", 1, 0);
			send(server3, "2", 1, 0);
			send(server3, &size_p1_buffer, 4, 0);
			send(server3, &size_p2_buffer, 4, 0);							
			send(server3, p1_buffer, size_p1_buffer, 0);
			send(server3, p2_buffer, size_p2_buffer, 0);

			//send pieces 2 and 3 to server 4
			send(server4, "2", 1, 0);
			send(server4, "3", 1, 0);
			send(server4, &size_p2_buffer, 4, 0);
			send(server4, &size_p3_buffer, 4, 0);							
			send(server4, p2_buffer, size_p2_buffer, 0);
			send(server4, p3_buffer, size_p3_buffer, 0);										
		}	

		else{ //key == 3
			//send pieces 2 and 3 to server 1
			send(server1, "2", 1, 0);
			send(server1, "3", 1, 0);
			send(server1, &size_p2_buffer, 4, 0);
			send(server1, &size_p3_buffer, 4, 0);							
			send(server1, p2_buffer, size_p2_buffer, 0);
			send(server1, p3_buffer, size_p3_buffer, 0);

			//send pieces 3 and 4 to server 2
			send(server2, "3", 1, 0);
			send(server2, "4", 1, 0);
			send(server2, &size_p3_buffer, 4, 0);
			send(server2, &size_p4_buffer, 4, 0);							
			send(server2, p3_buffer, size_p3_buffer, 0);
			send(server2, p4_buffer, size_p4_buffer, 0);	

			//send pieces 4 and 1 to server 3
			send(server3, "4", 1, 0);
			send(server3, "1", 1, 0);
			send(server3, &size_p4_buffer, 4, 0);
			send(server3, &size_p1_buffer, 4, 0);							
			send(server3, p4_buffer, size_p4_buffer, 0);
			send(server3, p1_buffer, size_p1_buffer, 0);

			//send pieces 1 and 2 to server 4
			send(server4, "1", 1, 0);
			send(server4, "2", 1, 0);
			send(server4, &size_p1_buffer, 4, 0);
			send(server4, &size_p2_buffer, 4, 0);							
			send(server4, p1_buffer, size_p1_buffer, 0);
			send(server4, p2_buffer, size_p2_buffer, 0);												
		}	
	}
}

int keyValueFromHash(char *hash){
	int v1, v2, v3, v4;
	sscanf( &hash[0], "%02x", &v1 );
	sscanf( &hash[8], "%02x", &v2 );
	sscanf( &hash[16], "%02x", &v3 );
	sscanf( &hash[24], "%02x", &v4 );

	int value;
	value = (v1 ^ v2 ^ v3 ^ v4) % 4;
	return value;
}

char *generateMD5Hash(char *filename){
    unsigned char c[MD5_DIGEST_LENGTH];
    int i;
    int j;
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("\n%s can't be opened\n", filename);
        return NULL;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0){
    	MD5_Update (&mdContext, data, bytes);
    }
 
	char *final_hash;
	final_hash = malloc(32);
	strcpy(final_hash, "");

	char *hash_byte;
	hash_byte = malloc(4);


    MD5_Final (c,&mdContext);
    for(i = 0; i < MD5_DIGEST_LENGTH; i++){
    	//printf("%02x", c[i]);
    	sprintf(hash_byte, "%02x", c[i]);
    	strcat(final_hash, hash_byte);
    } 

    fclose (inFile);
    return final_hash;
}

void putFile(char *filename){
	char *hashed_file;
	hashed_file = malloc(256);
	hashed_file = generateMD5Hash(filename);
	if(hashed_file != NULL){
		int key_val = keyValueFromHash(hashed_file);
		splitFileOntoServers(key_val, filename);		
	}
	else{ printf("%s doesn't exist\n", filename); }

}

void getFile(char *filename){
	retrieveFileFromServers(filename);
}

void listFiles(){
	printf("LIST command has not been implemented\n");
}

void dispatchCommand(char *command){
	int validUser;
	char *token = strtok(command, " \t\n");
	if(token != NULL){
		if(strcmp(token, "GET") == 0) {
			token = strtok(NULL, " \t\n");
			validUser = authenticate();
			if(validUser == 1){ getFile(token); }
			else{ printf("Invalid login credentials.\n"); }
		}
		else if(strcmp(token, "PUT") == 0){
			token = strtok(NULL, " \t\n");
			validUser = authenticate();
			if(validUser == 1) { putFile(token); }
			else{ printf("Invalid login credentials.\n"); }
		}
		else if(strcmp(token, "LIST") == 0){
			validUser = authenticate();
			if(validUser == 1){	listFiles(); }
			else{ printf("Invalid login credentials.\n"); }
		}
		else{
			printf("invalid command\n");
		}				
	}
}

int initializeConnection(int port){
	int sd;
	sd = socket(PF_INET, SOCK_STREAM, 0); /* create socket */
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));    /* create & zero struct */
	addr.sin_family = AF_INET;    /* select internet protocol */
	addr.sin_port = htons(port);         /* set the port # */
	addr.sin_addr.s_addr = INADDR_ANY; /* set the addr */
	if (connect(sd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
		printf("Could not connect to port %d on localhost\n", port);
		return 0;
	}
	else{
		printf("Connected to port %d on localhost\n", port);
		return sd;
	}
}

config_information parseConfigurationFile(char *filename){

	config_information conf_info;

	FILE *f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buffer = malloc(fsize + 1);
	fread(buffer, fsize, 1, f);
	fclose(f);

	char *token = strtok(buffer, " \t\n");
	while(token != NULL){
		if(strcmp(token, "Username:") == 0){
			token = strtok(NULL, " \t\n");
			strcpy(conf_info.usernames[usercount], token);
			token = strtok(NULL, " \t\n");
			if(strcmp(token, "Password:") == 0){
				token = strtok(NULL, " \t\n");
				strcpy(conf_info.passwords[usercount], token);
				usercount++;
			}
			else{
				printf("Configuration file incorrectly formatted\n");
			}
		}
		token = strtok(NULL, " \t\n");
	}

	return conf_info;
}

int main(int argc, char* argv[]){

	char *configuration_file;
	configuration_file = malloc(16);
	if(argc > 1){
		configuration_file = argv[1];
	}
	else{
		printf("USAGE: ./dfc <CONFIGURATION FILE>\n");
	}

	info = parseConfigurationFile(configuration_file);

	char *buffer;
	buffer = malloc(BUFFERSIZE);
	int validUser;

	server1 = initializeConnection(10001);
	server2 = initializeConnection(10002);
	server3 = initializeConnection(10003);
	server4 = initializeConnection(10004);

	validUser = authenticate(info);

	if(validUser != 0){
	   while(1){
		    printf("\nEnter a command: ");
		    if(fgets(buffer, BUFFERSIZE , stdin) != NULL){ dispatchCommand(buffer); }
		}		
	}
	else{
		printf("Invalid login credentials.\n");
		exit(0);
	}
	return 0;
}
