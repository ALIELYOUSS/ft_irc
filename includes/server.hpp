#ifndef SERVER_HPP
#define SERVER_HPP

#include "clients.hpp"
#include <unistd.h>
#include <string.h>
#include<iostream>
#include<sstream>
#include<vector> 
#include<map>
#include<poll.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/poll.h>
#include<sys/types.h>
#include <stdlib.h>

class server
{
private:
    int port;
    std::string passwd;
    int socket_file;
    std::map<int ,clinets> client;
    std::vector<pollfd> fds;
public:
    server(std::string port, std::string passwd);
    int init();
};

#endif