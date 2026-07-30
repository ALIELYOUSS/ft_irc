#include "../../includes/server.hpp"

std::string tolow(const std::string& str){
	std::string tmp;
	for (size_t i = 0; i < str.size(); i++){
		tmp.push_back(std::tolower(str[i]));
	}
	return tmp;
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
	if (tolow(cmpnts[cmpnt_index]._data) == "pass" || tolow(cmpnts[cmpnt_index]._data) == "nick" || tolow(cmpnts[cmpnt_index]._data) == "user")
		cl_registration(cl, tolow(cmpnts[cmpnt_index]._data));
	else if (tolow(cmpnts[cmpnt_index]._data) == "ping")
		cl_ping(cl);
	else if (tolow(cmpnts[cmpnt_index]._data) == "join")
		cl_join(cl);
	else if (tolow(cmpnts[cmpnt_index]._data) == "part")
		cl_part(cl);
	else if (tolow(cmpnts[cmpnt_index]._data )== "privmsg")
		cl_privmsg(cl);
	else if (tolow(cmpnts[cmpnt_index]._data) == "topic")
		cl_topic(cl);
	else if (tolow(cmpnts[cmpnt_index]._data) == "kick")
		cl_kick(cl);
	else if (tolow(cmpnts[cmpnt_index]._data) == "invite")
		cl_invite(cl);
	else if (tolow(cmpnts[cmpnt_index]._data) == "mode")
		cl_mode(cl);
	else
		cl.out_buf += ":server 421 " + cl.nickname + " " + tolow(cmpnts[cmpnt_index]._data) + " :Unknown command\r\n";
	cl.clearComponents();
}
