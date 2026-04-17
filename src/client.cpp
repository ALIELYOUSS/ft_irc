#include "../includes/clients.hpp"

void clinets::appendMsg(std::string msg, int size){
    this->buffer.append(msg, size);
}

bool clinets::hascompleteMg(){
    return buffer.find('/n') != std::string::npos;
}