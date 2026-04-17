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
	if (bind(socket_fd, (struct sockaddr *)&so, sizeof(so)) == -1){
		close(socket_fd);
		throw std::runtime_error("Failed to bind socket\n");
	}
	if (listen(socket_fd, 15)){
		close(socket_fd);
		throw std::runtime_error("Failed to listen on the socket\n");
	}
	this->socket_file = socket_fd;

	pollfd g;
	g.events = POLLIN;
	g.fd = this->socket_file;
	g.revents = 0;
	fds.push_back(g);

	while (true)
	{
		poll(fds.data(), fds.size(), -1);
		if (fds[0].events & POLLIN){
			int client_fd = accept4(this->socket_file, NULL, NULL);
		if (client_fd < 0){
			throw std::runtime_error("clien sockect failed\n");
		}
		{		
			pollfd b;
			b.events = POLLIN;
			b.fd = client_fd;
			b.revents = 0;
		}
		{
			Clients x;
			x.set_Values(this->socket_file);
			client.push_back(x);
		}
		for (size_t i = 0; i < fds.size(); i++){
			Clients &client = client[i - 1];
		}
	}	
	return 1;
};


server::server(std::string port, std::string passwd)
{
	this->port = atoi(port.c_str());
	if (this->port < 1024 || this->port > 65535)
		throw std::invalid_argument("Wrong Port\n");
	if (!is_wspace(passwd))
		throw std::invalid_argument("Wrong Passwrd\n");
	this->passwd = passwd;
}