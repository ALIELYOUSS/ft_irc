#include "../../includes/server.hpp"

void server::process_client_buffers()
{
	for (size_t i = 0; i < this->client.size(); ++i)
	{
		std::string line;
		while (this->client[i].popLine(line))
		{
			this->handle_client_line(this->client[i], line);
			this->handle_commands();
		}
	}
}