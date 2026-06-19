#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/poll.h>
#include<sys/types.h>
#include <stdlib.h>
#include <exception>
#include <iostream>

int main(){
    try {
        // open a socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            throw std::runtime_error("failed to open the socket");
        // the sockaddr_in struct
        
        struct sockaddr_in addrs;
        addrs.sin_family = AF_INET;
        addrs.sin_port = htons(8080);
        addrs.sin_addr.s_addr = INADDR_ANY;

        //  bind the socket in the kernel

        bind(sock, (struct sockaddr *)&addrs, sizeof(addrs));

        int client_sock = accept(sock, NULL, NULL);
        
        char buff[800];

        

    }
    catch (std::exception& error){
        error.what();
    }
}