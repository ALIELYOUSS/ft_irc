#include "../includes/server.hpp"

#include <cerrno>
#include <iomanip>

volatile sig_atomic_t server::running = 1;

static void debug_print_bytes(const std::string &label, const char *buf, size_t len)
{
	std::cout << "[DEBUG] " << label << " (" << len << " bytes): ";
	for (size_t i = 0; i < len && i < 100; ++i)
	{
		unsigned char c = buf[i];
		if (c == '\r')
			std::cout << "<CR>";
		else if (c == '\n')
			std::cout << "<LF>";
		else if (c >= 32 && c < 127)
			std::cout << c;
		else
			std::cout << "<" << std::setfill('0') << std::setw(2) << std::hex << (int)c << std::dec << std::setfill(' ') << ">";
	}
	std::cout << std::endl;
}

void server::handle_signal(int signum)
{
	if (signum == SIGINT)
		std::cout << "\n[SIGNAL] Caught SIGINT (Ctrl+C)" << std::endl;
	else if (signum == SIGTERM)
		std::cout << "\n[SIGNAL] Caught SIGTERM" << std::endl;
	else
		std::cout << "\n[SIGNAL] Caught signal " << signum << std::endl;
	server::running = 0;
}

pollfd server::make_pollfd(int fd)
{
	pollfd entry;
	entry.events = POLLIN;
	entry.fd = fd;
	entry.revents = 0;
	return entry;
}

void server::setup_listener()
{
	struct sockaddr_in address;
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons(this->port);
	address.sin_addr.s_addr = INADDR_ANY;

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		std::cerr << "[ERROR] socket() failed" << std::endl;
		throw std::runtime_error("socket_fd failed");
	}
	std::cout << "[SOCKET] Created listener socket fd=" << socket_fd << std::endl;

	int reuseAddr = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
	{
		close(socket_fd);
		std::cerr << "[ERROR] setsockopt(SO_REUSEADDR) failed" << std::endl;
		throw std::runtime_error("Failed to set SO_REUSEADDR\n");
	}
	std::cout << "[SOCKET] SO_REUSEADDR set" << std::endl;

	if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		close(socket_fd);
		std::cerr << "[ERROR] bind() failed on port " << this->port << std::endl;
		throw std::runtime_error("Failed to bind socket\n");
	}
	std::cout << "[SOCKET] Bound to 0.0.0.0:" << this->port << std::endl;

	if (listen(socket_fd, 15) == -1)
	{
		close(socket_fd);
		std::cerr << "[ERROR] listen() failed" << std::endl;
		throw std::runtime_error("Failed to listen on the socket\n");
	}
	std::cout << "[SOCKET] Listening (backlog=15)" << std::endl;

	this->socket_file = socket_fd;
	this->fds.push_back(make_pollfd(this->socket_file));
}

void server::install_signal_handlers()
{
	signal(SIGINT, server::handle_signal);
	signal(SIGTERM, server::handle_signal);
}

void server::close_socks(std::vector<pollfd> &fds)
{
	for (size_t i = 0; i < fds.size(); ++i)
		close(fds[i].fd);
	fds.clear();
}

void server::accept_client()
{
	int client_sock = accept(this->socket_file, NULL, NULL);
	if (client_sock < 0)
	{
		std::cerr << "[ERROR] accept() failed" << std::endl;
		throw std::runtime_error("client fd socket fail");
	}

	std::cout << "[ACCEPT] New client connected: fd=" << client_sock 
		<< " (total clients: " << (this->client.size() + 1) << ")" << std::endl;

	this->fds.push_back(make_pollfd(client_sock));

	Clients newClient;
	newClient.setValues(client_sock);
	this->client.push_back(newClient);
}

void server::remove_client(size_t index)
{
	int fd = this->fds[index].fd;
	std::cout << "[DISCONNECT] Client fd=" << fd 
		<< " disconnected (total clients: " << (this->client.size() - 1) << ")" << std::endl;
	close(this->fds[index].fd);
	this->fds.erase(this->fds.begin() + index);
	this->client.erase(this->client.begin() + (index - 1));
}

void server::handle_client_event(size_t index)
{
	int fd = this->fds[index].fd;

	if (this->fds[index].revents & POLLERR)
	{
		std::cout << "[POLL_ERROR] fd=" << fd << " got POLLERR" << std::endl;
		remove_client(index);
		return;
	}
	if (this->fds[index].revents & POLLHUP)
	{
		std::cout << "[POLL_ERROR] fd=" << fd << " got POLLHUP (client closed connection)" << std::endl;
		remove_client(index);
		return;
	}
	if (this->fds[index].revents & POLLNVAL)
	{
		std::cout << "[POLL_ERROR] fd=" << fd << " got POLLNVAL" << std::endl;
		remove_client(index);
		return;
	}

	if (!(this->fds[index].revents & POLLIN))
		return;

	char buf[512];
	ssize_t bytes = recv(this->fds[index].fd, buf, sizeof(buf), 0);
	if (bytes < 0)
	{
		std::cerr << "[ERROR] recv() failed on fd=" << fd << std::endl;
		remove_client(index);
		return;
	}
	if (bytes == 0)
	{
		std::cout << "[RECV] fd=" << fd << " sent EOF (connection closed)" << std::endl;
		remove_client(index);
		return;
	}

	std::cout << "[RECV] fd=" << fd << " received " << bytes << " bytes" << std::endl;
	debug_print_bytes("Raw data", buf, bytes);

	if (!this->client[index - 1].appendMsg(std::string(buf, bytes), bytes))
	{
		std::cout << "[BUFFER_FULL] fd=" << fd << " buffer overflow, disconnecting" << std::endl;
		remove_client(index);
		return;
	}

	std::string line;
	int lineCount = 0;
	while (this->client[index - 1].popLine(line))
	{
		lineCount++;
		std::cout << "[LINE] fd=" << fd << " extracted line #" << lineCount << std::endl;
		this->handle_client_line(this->client[index - 1], line);
	}
}

void server::run_event_loop()
{
	std::cout << "[LOOP] Starting poll loop with " << this->fds.size() << " fd(s)" << std::endl;
	while (server::running)
	{
		std::cout << "[LOOP] poll() waiting on " << this->fds.size() << " fd(s)..." << std::endl;
		int ready = poll(this->fds.data(), this->fds.size(), -1);
		if (ready < 0)
		{
			if (!server::running && errno == EINTR)
			{
				std::cout << "[LOOP] poll() interrupted after shutdown signal" << std::endl;
				break;
			}
			if (errno == EINTR)
			{
				std::cout << "[LOOP] poll() interrupted, retrying" << std::endl;
				continue;
			}
			std::cerr << "[ERROR] poll() failed" << std::endl;
			throw std::runtime_error("poll failed\n");
		}
		if (ready == 0)
		{
			std::cout << "[LOOP] poll() timeout (shouldn't happen with -1 timeout)" << std::endl;
			continue;
		}

		std::cout << "[LOOP] poll() returned " << ready << " ready fd(s)" << std::endl;

		if (this->fds[0].revents & POLLIN)
		{
			std::cout << "[LOOP] Listener fd ready, accepting connection..." << std::endl;
			accept_client();
		}

		for (size_t i = 1; i < this->fds.size();)
		{
			if (this->fds[i].revents)
				std::cout << "[LOOP] Client fd=" << this->fds[i].fd << " has events" << std::endl;
			size_t currentSize = this->fds.size();
			handle_client_event(i);
			if (this->fds.size() == currentSize)
				++i;
		}
	}
	std::cout << "[LOOP] Event loop ended" << std::endl;
}