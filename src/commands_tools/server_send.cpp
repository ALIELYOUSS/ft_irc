#include "../../includes/server.hpp"
#include <cerrno>

bool server::send_msg(Clients &client)
{
	while (!client.out_buf.empty())
	{
		ssize_t sent = send(client.getFd(), client.out_buf.c_str(), client.out_buf.size(), MSG_NOSIGNAL);
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
		if (this->client[i].out_buf.size() > kMaxOutputBufferBytes)
		{
			std::cerr << "[BUFFER_OVERFLOW] Client fd=" << this->client[i].getFd()
				<< " output buffer exceeded " << kMaxOutputBufferBytes
				<< " bytes (" << this->client[i].out_buf.size() << "), disconnecting" << std::endl;
			for (size_t j = 0; j < this->fds.size(); ++j)
			{
				if (this->fds[j].fd == this->client[i].getFd()){
					remove_client(j);
					break;
				}
			}
			if (i > 0) --i;
			continue;
		}
		if (this->client[i].out_buf.empty())
			continue;
		if (!send_msg(this->client[i])){
			std::cerr << "send() error\n";
			this->client[i].out_buf.clear();
		}
		if (!this->client[i].out_buf.empty())
		{
			int fd = this->client[i].getFd();
			for (size_t j = 0; j < this->fds.size(); ++j)
			{
				if (this->fds[j].fd == fd)
				{
					this->fds[j].events |= POLLOUT;
					break;
				}
			}
		}
	}
}
