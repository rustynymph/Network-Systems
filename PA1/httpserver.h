void initializeServer(char *port);
void *listenForRequests(int socket);
void handleGETRequest(char *filename, int socket);
void sendHTMLPage(int socket, char *filename);
void *sendGIF(void *arguments);
void *sendPNG(void *arguments);
void *sendJPG(void *arguments);
void *sendCSS(void *arguments);
void *sendText(void *arguments);
void *sendJavaScript(void *arguments);
const char *get_filename_ext(const char *filename);
void parseHTTPrequest(char *request, int socket);
void respond(int n);