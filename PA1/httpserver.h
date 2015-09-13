void initializeServer(int port);
void *listenForRequests(void *socket);
void handleGETRequest(char *filename, int socket);
void sendHTMLPage(int socket, char *filename);
void sendGIF(int socket, char *filename);
void sendText(int socket, char *filename);
void sendJavaScript(int socket, char *filename);
const char *get_filename_ext(const char *filename);
void parseHTTPrequest(char *request, int socket);