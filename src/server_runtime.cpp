#include "../includes/server.hpp"

#include <cerrno>

volatile sig_atomic_t server::running = 1;

void server::handle_signal(int signum)
{
	(void)signum;
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
		throw std::runtime_error("socket_fd failed");

	int reuseAddr = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
	{
		close(socket_fd);
		throw std::runtime_error("Failed to set SO_REUSEADDR\n");
	}
	if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		close(socket_fd);
		throw std::runtime_error("Failed to bind socket\n");
	}
	if (listen(socket_fd, 15) == -1)
	{
		close(socket_fd);
		throw std::runtime_error("Failed to listen on the socket\n");
	}

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
		throw std::runtime_error("client fd socket fail");

	this->fds.push_back(make_pollfd(client_sock));

	Clients newClient;
	newClient.setValues(client_sock);
	this->client.push_back(newClient);
}

void server::remove_client(size_t index)
{
	close(this->fds[index].fd);
	this->fds.erase(this->fds.begin() + index);
	this->client.erase(this->client.begin() + (index - 1));
}

void server::handle_client_event(size_t index)
{
	if (this->fds[index].revents & (POLLERR | POLLHUP | POLLNVAL))
	{
		remove_client(index);
		return;
	}

	if (!(this->fds[index].revents & POLLIN))
		return;

	char buf[512];
	ssize_t bytes = recv(this->fds[index].fd, buf, sizeof(buf), 0);
	if (bytes <= 0)
	{
		remove_client(index);
		return;
	}

	if (!this->client[index - 1].appendMsg(std::string(buf, bytes), bytes))
	{
		remove_client(index);
		return;
	}

	std::string line;
	while (this->client[index - 1].popLine(line))
		this->handle_client_line(this->client[index - 1], line);
}

void server::run_event_loop()
{
	while (server::running)
	{
		int ready = poll(this->fds.data(), this->fds.size(), -1);
		if (ready < 0)
		{
			if (!server::running && errno == EINTR)
				break;
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll failed\n");
		}
		if (ready == 0)
			continue;

		if (this->fds[0].revents & POLLIN)
			accept_client();

		for (size_t i = 1; i < this->fds.size();)
		{
			size_t currentSize = this->fds.size();
			handle_client_event(i);
			if (this->fds.size() == currentSize)
				++i;
		}
	}
}