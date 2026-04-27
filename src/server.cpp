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
	std::cout << "[INIT] Server constructed. Port: " << this->port << ", Password: " << this->passwd << std::endl;
}

int server::init(){
	std::cout << "[INIT] Starting server initialization..." << std::endl;
	this->setup_listener();
	std::cout << "[INIT] Listener setup complete on port " << this->port << std::endl;
	this->install_signal_handlers();
	std::cout << "[INIT] Signal handlers installed (SIGINT, SIGTERM)" << std::endl;
	server::running = 1;
	std::cout << "[INIT] Entering event loop..." << std::endl;
	this->run_event_loop();
	std::cout << "[SHUTDOWN] Event loop ended, closing sockets..." << std::endl;
	close_socks(this->fds);
	this->client.clear();
	std::cout << "[SHUTDOWN] All clients cleared. Server stopped." << std::endl;
	return 1;
}
