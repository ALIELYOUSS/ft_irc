#include "../includes/clients.hpp"

void Clients::appendMsg(std::string msg, int size){
    this->buffer.append(msg, size);
}

bool Clients::hascompleteMg(){
    return buffer.find('/n') != std::string::npos;
}

void Clients::set_Values(int fd){
    this->client_fd;
    this->buffer = "";
}