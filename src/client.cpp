#include "../includes/clients.hpp"

namespace
{
    const size_t kMaxClientBufferBytes = 8192;
}

bool Clients::appendMsg(const std::string &msg, int size){
    if (size < 0)
        return false;
    if (this->buffer.size() + static_cast<size_t>(size) > kMaxClientBufferBytes)
        return false;
    this->buffer.append(msg, size);
    return true;
}

bool Clients::hasCompleteMessage(){
    return buffer.find('\n') != std::string::npos;
}

bool Clients::popLine(std::string &line)
{
    size_t pos = this->buffer.find('\n');
    if (pos == std::string::npos)
        return false;
    line = this->buffer.substr(0, pos);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    this->buffer.erase(0, pos + 1);
    return true;
}

int Clients::getFd() const
{
    return this->client_fd;
}

void Clients::setValues(int fd){
    this->client_fd = fd;
    this->buffer = "";
}