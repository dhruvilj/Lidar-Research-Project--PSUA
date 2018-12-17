#include <iostream>
#include <stdlib.h>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"

using namespace std;

void checkError(int code, string message, string success) {
    if (code ==-1) {
        cout << message << endl;
        exit(1);
    }
    cout << success <<endl;
}

void getMessage(int socketfd, void* buffer) {
    recv(socketfd,buffer,BUFF_SIZE,0);
}

void sendMessageCLI(int socketfd) {
    char buffer[BUFF_SIZE];
    cin.getline(buffer, BUFF_SIZE);
    send(socketfd,buffer,5,0);
}

void sendMessage(int socketfd, char* message,int size) {
    send(socketfd,message,size,0);
}
