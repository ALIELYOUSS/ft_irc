#include "../../includes/server.hpp"

void server::cl_ping(Clients &client)
{
	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;

	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;
	if (cmpnts.size() < cmpnt_index + 2)
	{
		client.out_buf = "PONG\r\n";
		return;
	}
	std::string payload;
	for (size_t j = cmpnt_index + 1; j < cmpnts.size(); ++j)
	{
		if (cmpnts[j]._type == TRAILING || cmpnts[j]._type == MIDDLE)
		{
			payload = cmpnts[j]._data;
			break;
		}
	}
	client.out_buf = "PONG :" + payload + "\r\n";
}
