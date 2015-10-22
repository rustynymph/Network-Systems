Author: Annie Kelly

Date: 10/22/15

Course: Network Systems CSCI4273


Usage
=====
Make sure all servers are running before starting the client!
When adding a username and password to the dfc.conf file please
follow this format


Username: my_username

Password: my_password


When using GET, PUT, and LIST make sure the command is capitalized.  As of now
LIST has not be implemented.


Compiling and Running the servers
=================================
gcc -o dfs dfs.c

./dfs <DIRECTORY> <PORT NUMBER>


*Note:* when specifying the directory, do not put a slash in front. 
This will cause the program to use this as an absolute filepath
instead of a relative one.  For example use "DFS1" not "/DFS1".

Compiling and Running the client
================================
gcc -o dfc dfc.c -lcrypto -lssl

./dfc dfc.conf

