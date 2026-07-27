#include "../../includes/server.hpp"

void server::cl_privmsg(Clients &client)
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

	if (cmpnts.size() < cmpnt_index + 2)
	{
		client.out_buf += "ERR_NORECIPIENT\r\n";
		return;
	}

	if (cmpnts[cmpnt_index + 1]._type != MIDDLE)
	{
		client.out_buf += "ERR_NORECIPIENT\r\n";
		return;
	}

	if (cmpnts.size() < cmpnt_index + 3)
	{
		client.out_buf += "ERR_NOTEXTTOSEND\r\n";
		return;
	}

	std::string text;
	size_t last_idx = cmpnts.size() - 1;
	if (cmpnts[last_idx]._type == TRAILING)
		text = cmpnts[last_idx]._data;
	else if (cmpnts[last_idx]._type == MIDDLE)
		text = cmpnts[last_idx]._data;
	else
	{
		client.out_buf += "ERR_NOTEXTTOSEND\r\n";
		return;
	}
	if (text.empty())
	{
		client.out_buf += "ERR_NOTEXTTOSEND\r\n";
		return;
	}

	std::vector<std::string> targets = splitByComma(cmpnts[cmpnt_index + 1]._data);

	for (size_t i = 0; i < targets.size(); ++i)
	{
		std::string target = targets[i];
		if (target.empty())
			continue;

		if (target[0] == '#' || target[0] == '&')
		{
			std::map<std::string, Channel>::iterator it = this->channels.find(target);
			if (it == this->channels.end())
			{
				client.out_buf += "ERR_CANNOTSENDTOCHAN\r\n";
				continue;
			}
			Channel &channel = it->second;
			if (!channel.isMember(client.getFd()))
			{
				client.out_buf += "ERR_CANNOTSENDTOCHAN\r\n";
				continue;
			}
			std::string msg = ":" + client.nickname + "!" + client.username + "@localhost PRIVMSG " + target + " :" + text + "\r\n";
			const std::set<int> &members = channel.getMembers();
			for (std::set<int>::iterator it2 = members.begin(); it2 != members.end(); ++it2)
			{
				int memberFd = *it2;
				if (memberFd == client.getFd())
					continue;
				for (size_t j = 0; j < this->client.size(); ++j)
				{
					if (this->client[j].getFd() == memberFd)
					{
						this->client[j].out_buf += msg;
						break;
					}
				}
			}
		}
		else
		{
			bool found = false;
			for (size_t j = 0; j < this->client.size(); ++j)
			{
				if (this->client[j].nickname == target)
				{
					std::string msg = ":" + client.nickname + "!" + client.username + "@localhost PRIVMSG " + target + " :" + text + "\r\n";
					this->client[j].out_buf += msg;
					found = true;
					break;
				}
			}
			if (!found)
				client.out_buf += "ERR_NOSUCHNICK\r\n";
		}
	}
}

