#include "../../includes/server.hpp"

void server::cl_part(Clients &client)
{
	if (!client.registred)
	{
		client.out_buf += "ERR_NOTREGISTERED\r\n";
		return;
	}

	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;

	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;
	if (cmpnts.size() < cmpnt_index + 2 || cmpnts[cmpnt_index + 1]._type != MIDDLE)
	{
		client.out_buf += "ERR_NEEDMOREPARAMS\r\n";
		return;
	}
	std::vector<std::string> channels = splitByComma(cmpnts[cmpnt_index + 1]._data);
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string chanName = channels[i];
		std::map<std::string, Channel>::iterator it = this->channels.find(chanName);
		if (it == this->channels.end())
		{
			client.out_buf += "ERR_NOSUCHCHANNEL\r\n";
			continue;
		}
		Channel &channel = it->second;
		if (!channel.isMember(client.getFd()))
		{
			client.out_buf += "ERR_NOTONCHANNEL\r\n";
			continue;
		}
		std::string partMsg = ":" + client.nickname + "!" + client.username + "@localhost PART " + chanName + "\r\n";
		const std::set<int> &members = channel.getMembers();
		for (std::set<int>::iterator it2 = members.begin(); it2 != members.end(); ++it2)
		{
			int memberFd = *it2;
			for (size_t j = 0; j < this->client.size(); ++j)
			{
				if (this->client[j].getFd() == memberFd)
				{
					this->client[j].out_buf += partMsg;
					break;
				}
			}
		}
		channel.removeMember(client.getFd());
		channel.removeOperator(client.getFd());
		if (channel.memberCount() == 0)
			this->channels.erase(it);
	}
}

