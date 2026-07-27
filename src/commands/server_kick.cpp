#include "../../includes/server.hpp"

void server::cl_kick(Clients &client)
{
	if (!client.registred)
	{
		client.out_buf += ":server 451 " + client.nickname + " KICK :You have not registered\r\n";
		return;
	}

	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;

	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;

	if (cmpnts.size() < cmpnt_index + 2 || cmpnts[cmpnt_index + 1]._type != MIDDLE)
	{
		client.out_buf += ":server 461 " + client.nickname + " KICK :Not enough parameters\r\n";
		return;
	}

	std::string chanName = cmpnts[cmpnt_index + 1]._data;
	std::map<std::string, Channel>::iterator it = this->channels.find(chanName);

	if (it == this->channels.end())
	{
		client.out_buf += ":server 403 " + client.nickname + " " + chanName + " :No such channel\r\n";
		return;
	}

	Channel &channel = it->second;

	if (!channel.isMember(client.getFd()))
	{
		client.out_buf += ":server 442 " + client.nickname + " " + chanName + " :You're not on that channel\r\n";
		return;
	}

	if (!channel.isOperator(client.getFd()))
	{
		client.out_buf += ":server 482 " + client.nickname + " " + chanName + " :You're not channel operator\r\n";
		return;
	}

	if (cmpnts.size() < cmpnt_index + 3)
	{
		client.out_buf += ":server 461 " + client.nickname + " KICK :Not enough parameters\r\n";
		return;
	}

	std::string targetStr;
	if (cmpnts[cmpnt_index + 2]._type == MIDDLE)
		targetStr = cmpnts[cmpnt_index + 2]._data;
	else
	{
		client.out_buf += ":server 461 " + client.nickname + " KICK :Not enough parameters\r\n";
		return;
	}

	std::vector<std::string> targets = splitByComma(targetStr);

	std::string reason;
	size_t last_idx = cmpnts.size() - 1;
	if (last_idx > cmpnt_index + 2 && cmpnts[last_idx]._type == TRAILING)
		reason = cmpnts[last_idx]._data;
	if (reason.empty())
		reason = client.nickname;

	for (size_t i = 0; i < targets.size(); ++i)
	{
		std::string targetNick = targets[i];

		int targetFd = -1;
		for (size_t j = 0; j < this->client.size(); ++j)
		{
			if (this->client[j].nickname == targetNick)
			{
				targetFd = this->client[j].getFd();
				break;
			}
		}

		if (targetFd == -1)
		{
			client.out_buf += ":server 401 " + client.nickname + " " + targetNick + " :No such nick/channel\r\n";
			continue;
		}

		if (!channel.isMember(targetFd))
		{
			client.out_buf += ":server 441 " + client.nickname + " " + targetNick + " " + chanName + " :They aren't on that channel\r\n";
			continue;
		}

		std::string kickMsg = ":" + client.nickname + "!" + client.username + "@localhost KICK " + chanName + " " + targetNick + " :" + reason + "\r\n";

		const std::set<int> &members = channel.getMembers();
		for (std::set<int>::iterator it2 = members.begin(); it2 != members.end(); ++it2)
		{
			int memberFd = *it2;
			for (size_t j = 0; j < this->client.size(); ++j)
			{
				if (this->client[j].getFd() == memberFd)
				{
					this->client[j].out_buf += kickMsg;
					break;
				}
			}
		}
		channel.removeMember(targetFd);
		channel.removeOperator(targetFd);

		if (channel.memberCount() == 0)
			this->channels.erase(it);
	}
}
