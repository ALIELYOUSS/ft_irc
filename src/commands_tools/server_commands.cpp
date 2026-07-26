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

void server::handle_commands(size_t client_index)
{
	if (client_index >= this->client.size())
		return;
	Clients &cl = this->client[client_index];
	const std::vector<t_cmpnts> &cmpnts = cl.getComponents();
	size_t cmpnt_index = 0;

	if (cmpnts.empty())
		return;
	if (cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;
	if (cmpnt_index >= cmpnts.size() || cmpnts[cmpnt_index]._type != CMD)
	{
		cl.clearComponents();
		return;
	}
	if (cmpnts[cmpnt_index]._data == "PASS" || cmpnts[cmpnt_index]._data == "NICK" || cmpnts[cmpnt_index]._data == "USER")
		cl_registration(cl, cmpnts[cmpnt_index]._data);
	if (cmpnts[cmpnt_index]._data == "PING")
		cl_ping(cl);
	cl.clearComponents();
}
