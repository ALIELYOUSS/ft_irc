#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include <iostream>
#include <string>

class Clients
{
private:
    int client_fd;
    std::string buffer;
public:
    Clients(){};
    ~Clients(){};
    bool appendMsg(const std::string &msg, int size);
    bool hasCompleteMessage();
    bool popLine(std::string &line);
    int getFd() const;
    void setValues(int fd);
};

#endif