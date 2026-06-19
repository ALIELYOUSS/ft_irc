#include "../includes/server.hpp"

void server::handle_client_line(Clients &cl, const std::string &line)
{
	if (line.empty())
		return;
	std::cout << "[CLIENT fd" << cl.getFd() << "] " << line << std::endl;
}

server::server(std::string port, std::string passwd)
{
	this->port = parsePort(port);
	if (!passwordHasNoWhitespace(passwd))
		throw std::invalid_argument("No white spaces allowed in the Passwrd\n");
	this->passwd = passwd;
}

int server::init(){
	std::cout << "Starting server initialization..." << std::endl;
	this->setup_listener();
	this->install_signal_handlers();
	server::running = 1;
	this->run_event_loop();
	std::cout << "Event loop ended, closing sockets..." << std::endl;
	close_socks(this->fds);
	this->client.clear();
	std::cout << "All clients cleared. Server stopped." << std::endl;
	return 1;
}
