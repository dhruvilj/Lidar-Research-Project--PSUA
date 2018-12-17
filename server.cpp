#include <iostream>
#include <string>
#include<functional>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "server.h"

using namespace std;

Server::Server(int port_num) {
    isConnect=false;
    listener=socket(AF_INET,SOCK_STREAM,0); 
    checkError(listener,"Error initializing socket!", "Socket initialized");

    server_address.sin_family=AF_INET; 
    server_address.sin_addr.s_addr=htons(INADDR_ANY); 
    server_address.sin_port=htons(port_num);

    int status=bind(listener,(struct sockaddr *) &server_address, sizeof(server_address)); 
    checkError(status,"Error assigning protocol address to socket","Successfully assigning protocol to socket");
    
    status=listen(listener,MAX_QUEUE);
    checkError(status,"Error switching to LISTEN state","Socket now accepts incoming request");
}

void Server::waitForClient() {
    if (isConnect) {
        throw invalid_argument( "Another client has already connected" );
    }
    client_len=sizeof(client_address); //cout << "1" <<endl;
    connection=accept(listener,(struct sockaddr * ) &client_address, &client_len); 
    checkError(connection,"Error connecting client", "Connection accepted");
    child_id=fork();
    if (child_id==0) {
        close(listener);
        isConnect=true;
    }
}

void Server::closeConnection() {
    close(connection);
    isConnect=false; //Open to another client
}

void Server::sendToClient(char * buffer) {
    sendMessage(connection,buffer,sizeof(buffer)/sizeof(buffer[0]));
}

void Server::sendToClientCLI() {
    sendMessageCLI(connection);
}

void Server::getClientMessage(char *buff) {
    getMessage(connection,buff);
}

void Server::sendToClientStr(string message) {
    char* messgChr=(char *) message.c_str();
    sendMessage(connection,messgChr,message.length());
}

Server::~Server() {
    close(connection);
}
