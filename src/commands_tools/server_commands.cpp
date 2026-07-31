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
	std::string cmd = tolow(cmpnts[cmpnt_index]._data);
	if (cmd == "pass" || cmd == "nick" || cmd == "user")
		cl_registration(cl, cmd);
	else if (cmd == "ping")
		cl_ping(cl);
	else if (cmd == "join")
		cl_join(cl);
	else if (cmd == "part")
		cl_part(cl);
	else if (cmd == "privmsg")
		cl_privmsg(cl);
	else if (cmd == "topic")
		cl_topic(cl);
	else if (cmd == "kick")
		cl_kick(cl);
	else if (cmd == "invite")
		cl_invite(cl);
	else if (cmd == "mode")
		cl_mode(cl);
	else
		cl.out_buf += ":server 421 " + cl.nickname + " " + cmpnts[cmpnt_index]._data + " :Unknown command\r\n";
	cl.clearComponents();
}
