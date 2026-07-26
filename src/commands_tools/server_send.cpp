#include "../../includes/server.hpp"

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