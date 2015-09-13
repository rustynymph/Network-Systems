void initializeServer(int port);
void *listenForRequests(void *socket);
void handleGETRequest(char *request);
void sendHTMLPage(int socket, char *filename);
void sendImage(int socket, char *filename);
void sendText(int socket, char *filename);