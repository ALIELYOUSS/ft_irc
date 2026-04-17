#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include "server.hpp"

class clinets
{
private:
    int client_fd;
    std::string buffer;
public:
    void appendMsg(std::string msg, int size);
    bool hascompleteMg();
};

#endif