#include "../../includes/server.hpp"

void server::cl_ping(Clients &client)
{
    std::cout << "PING handler called\n";
	client.out_buf = "PONG\r\n";
}