#include "../../includes/server.hpp"

bool server::send_msg(Clients &client)
{
	while (!client.out_buf.empty())
	{
		ssize_t sent = send(client.getFd(), client.out_buf.c_str(), client.out_buf.size(), 0);
		if (sent < 0)
			return false;
		client.out_buf.erase(0, static_cast<size_t>(sent));
	}
	return true;
}

void server::flush_out_buffers()
{
	for (size_t i = 0; i < this->client.size(); ++i)
	{
		if (this->client[i].out_buf.empty())
			continue;
		if (!send_msg(this->client[i]))
			std::cerr << "send() error\n";
		this->client[i].out_buf.clear();
	}
}
