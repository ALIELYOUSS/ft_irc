#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include "server.hpp"

class Clients
{
private:
    int client_fd;
    std::string buffer;
public:
    Clients(){};
    ~Clients(){};
    void appendMsg(std::string msg, int size);
    bool hascompleteMg();
    void set_Values(int fd);
};

#endif