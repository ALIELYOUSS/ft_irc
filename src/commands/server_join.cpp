#include "../../includes/server.hpp"

#include <sstream>

void server::cl_join(Clients &client)
{
	if (!client.registred)
	{
		client.out_buf += ":server 451 " + client.nickname + " JOIN :You have not registered\r\n";
		return;
	}

	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;

	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;

	if (cmpnts.size() < cmpnt_index + 2)
	{
		client.out_buf += ":server 461 " + client.nickname + " JOIN :Not enough parameters\r\n";
		return;
	}

	if (cmpnts[cmpnt_index + 1]._type != MIDDLE)
	{
		client.out_buf += ":server 461 " + client.nickname + " JOIN :Not enough parameters\r\n";
		return;
	}

	std::vector<std::string> channels = splitByComma(cmpnts[cmpnt_index + 1]._data);
	std::vector<std::string> keys;
	if (cmpnts.size() >= cmpnt_index + 3 && cmpnts[cmpnt_index + 2]._type == MIDDLE)
		keys = splitByComma(cmpnts[cmpnt_index + 2]._data);

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string chanName = channels[i];
		std::string key;
		if (i < keys.size())
			key = keys[i];
		if (chanName.empty() || chanName[0] != '#')
		{
			client.out_buf += ":server 476 " + client.nickname + " " + chanName + " :Bad Channel Mask\r\n";
			continue;
		}
		if (this->channels.find(chanName) == this->channels.end())
			this->channels[chanName] = Channel(chanName);
		Channel &channel = this->channels[chanName];
		if (!channel.canJoin(client.getFd(), key))
		{
			if (channel.isMember(client.getFd()))
				continue;
			else if (channel.isInviteOnly())
				client.out_buf += ":server 473 " + client.nickname + " " + chanName + " :Cannot join channel (+i)\r\n";
			else if (!channel.getKey().empty() && channel.getKey() != key)
				client.out_buf += ":server 475 " + client.nickname + " " + chanName + " :Cannot join channel (+k)\r\n";
			else if (channel.getLimit() != 0 && channel.memberCount() >= channel.getLimit())
				client.out_buf += ":server 471 " + client.nickname + " " + chanName + " :Cannot join channel (+l)\r\n";
			else
				client.out_buf += ":server 476 " + client.nickname + " " + chanName + " :Bad Channel Mask\r\n";
			continue;
		}

		channel.addMember(client.getFd());
		if (channel.memberCount() == 1)
			channel.addOperator(client.getFd());

		std::string joinMsg = ":" + client.nickname + "!" + client.username + "@localhost JOIN " + chanName + "\r\n";
		const std::set<int> &members = channel.getMembers();
		for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it)
		{
			int memberFd = *it;
			for (size_t j = 0; j < this->client.size(); ++j)
			{
				if (this->client[j].getFd() == memberFd)
				{
					this->client[j].out_buf += joinMsg;
					break;
				}
			}
		}

		if (!channel.getTopic().empty())
		{
			client.out_buf += ":server 332 " + client.nickname + " " + chanName + " :" + channel.getTopic() + "\r\n";
		}

		client.out_buf += ":server 353 " + client.nickname + " = " + chanName + " :";
		for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it)
		{
			int memberFd = *it;
			std::string nick;
			for (size_t j = 0; j < this->client.size(); ++j)
			{
				if (this->client[j].getFd() == memberFd)
				{
					nick = this->client[j].nickname;
					break;
				}
			}
			if (channel.isOperator(memberFd))
				client.out_buf += "@";
			client.out_buf += nick + " ";
		}
		client.out_buf += "\r\n";
		client.out_buf += ":server 366 " + client.nickname + " " + chanName + " :End of /NAMES list.\r\n";
	}
}
