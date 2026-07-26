#include "../../includes/server.hpp"

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
	else if (cmpnts[cmpnt_index]._data == "PING")
		cl_ping(cl);
	else if (cmpnts[cmpnt_index]._data == "JOIN")
		cl_join(cl);
	cl.clearComponents();
}
