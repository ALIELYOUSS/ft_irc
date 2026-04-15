#include "../includes/server.hpp"

static int is_wspace(const std::string &passwd)
{
	for (size_t i = 0; i < passwd.size(); i++)
	{
		if ((passwd[i] >= 9 && passwd[i] <= 13) || passwd[i] == ' ')
			return 0;
	}
	return 1;
}

int server::init(){
    struct sockaddr_in so;
    memset(&so, 0 , sizeof(struct sockaddr_in));
    so.sin_family = AF_INET;
    so.sin_port = htons(this->port);
    so.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1){
        throw std::runtime_error("socket_fd failed");
	}
	return 1;
}


server::server(std::string port, std::string passwd)
{
	this->port = atoi(port.c_str());
	if (this->port < 1024 || this->port > 65535)
		throw std::invalid_argument("Wrong Port\n");
	if (!is_wspace(passwd))
		throw std::invalid_argument("Wrong Passwrd\n");
	this->passwd = passwd;
}


