#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include <iostream>
#include <string>
#include <vector>

typedef enum e_cmpnt_type
{
	PREFIX,
	CMD,
	MIDDLE,
	TRAILING
} t_cmpnt_type;

typedef struct s_cmpnts
{
	std::string _data;
	t_cmpnt_type _type;
} t_cmpnts;

class Clients
{
private:
    int client_fd;
    std::string buffer;
    std::vector<t_cmpnts> components;
public:
    std::string nickname;
    std::string username;
    std::string password;
    std::string out_buf;
    bool registred;
    bool announced;

    Clients();
    explicit Clients(int fd);
    ~Clients(){};
    bool appendMsg(const std::string &msg, int size);
    bool hasCompleteMessage();
    bool popLine(std::string &line);
    int getFd() const;
    void setValues(int fd);
    void tokenizeMessage(const std::string &line);
    void clearComponents();
    const std::vector<t_cmpnts> &getComponents() const;
};

#endif