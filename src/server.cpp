#include "../includes/server.hpp"

void server::handle_client_line(Clients &cl, const std::string &line)
{
	if (line.empty())
		return;
	if (!cl.announced)
	{
		std::cout << "[IRC] client fd=" << cl.getFd() << " connected to irc server, buffer sent: " << line << std::endl;
		cl.announced = true;
	}
	cl.tokenizeMessage(line);
}

server::server(std::string port, std::string passwd)
{
	this->port = parsePort(port);
	if (!passwordHasNoWhitespace(passwd))
		throw std::invalid_argument("No white spaces allowed in the Passwrd\n");
	this->passwd = passwd;
}

int server::init(){
	this->setup_listener();
	this->install_signal_handlers();
	server::running = 1;
	this->run_event_loop();
	close_socks(this->fds);
	this->client.clear();
	return 1;
}
