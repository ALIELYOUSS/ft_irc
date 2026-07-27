#include "../includes/clients.hpp"

static const size_t kMaxClientBufferBytes = 8192;

static void pushComponent(std::vector<t_cmpnts> &components, const std::string &data, t_cmpnt_type type)
{
    t_cmpnts component;
    component._data = data;
    component._type = type;
    components.push_back(component);
}

static bool isMiddleComponent(const std::string &value)
{
    if (value.empty() || value[0] == ':')
        return false;
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (value[i] == ' ' || value[i] == '\0' || value[i] == '\r' || value[i] == '\n')
            return false;
    }
    return true;
}

static bool isTrailingComponent(const std::string &value)
{
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (value[i] == '\0' || value[i] == '\r' || value[i] == '\n')
            return false;
    }
    return true;
}

Clients::Clients() : client_fd(-1), buffer(""), components(), nickname(""), username(""), password(""), out_buf(""), registred(false)
{
}

Clients::Clients(int fd) : client_fd(fd), buffer(""), components(), nickname(""), username(""), password(""), out_buf(""), registred(false)
{
}

bool Clients::appendMsg(const std::string &msg, int size){
    if (size < 0)
        return false;
    if (this->buffer.size() + static_cast<size_t>(size) > kMaxClientBufferBytes)
        return false;
    this->buffer.append(msg, 0, size);
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
    this->components.clear();
    this->nickname = "";
    this->username = "";
    this->password = "";
    this->out_buf = "";
    this->registred = false;
}

void Clients::clearComponents()
{
    this->components.clear();
}

const std::vector<t_cmpnts> &Clients::getComponents() const
{
    return this->components;
}

void Clients::tokenizeMessage(const std::string &line)
{
    size_t i = 0;
    bool hasPrefix = false;
    bool hasCommand = false;

    this->clearComponents();
    while (i < line.size() && line[i] == ' ')
        ++i;
    while (i < line.size())
    {
        while (i < line.size() && line[i] == ' ')
            ++i;
        if (i >= line.size())
            break;
        if (line[i] == ':' && !hasPrefix && !hasCommand)
        {
            size_t start = ++i;
            while (i < line.size() && line[i] != ' ')
                ++i;
            pushComponent(this->components, line.substr(start, i - start), PREFIX);
            hasPrefix = true;
            continue;
        }

        size_t start = i;
        while (i < line.size() && line[i] != ' ')
            ++i;
        if (!hasCommand)
        {
            pushComponent(this->components, line.substr(start, i - start), CMD);
            hasCommand = true;
        }
        else if (line[start] == ':')
        {
            std::string trailing = line.substr(start + 1);
            if (isTrailingComponent(trailing))
                pushComponent(this->components, trailing, TRAILING);
            break;
        }
        else
        {
            std::string middle = line.substr(start, i - start);
            if (isMiddleComponent(middle))
                pushComponent(this->components, middle, MIDDLE);
        }
    }
}
