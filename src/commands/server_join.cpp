#include "../../includes/server.hpp"

#include <sstream>

static std::vector<std::string> splitByComma(const std::string &s)
{
	std::vector<std::string> result;
	std::string current;
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == ',')
		{
			if (!current.empty())
				result.push_back(current);
			current.clear();
		}
		else
			current += s[i];
	}
	if (!current.empty())
		result.push_back(current);
	return result;
}

void server::cl_join(Clients &client)
{
	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;

	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;

	// Need at least CMD + 1 param (channel names)
	if (cmpnts.size() < cmpnt_index + 2)
	{
		client.out_buf += "ERR_NEEDMOREPARAMS\r\n";
		return;
	}

	// First param after command must be MIDDLE (channel list)
	if (cmpnts[cmpnt_index + 1]._type != MIDDLE)
	{
		client.out_buf += "ERR_NEEDMOREPARAMS\r\n";
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

		// Validate channel name starts with '#'
		if (chanName.empty() || chanName[0] != '#')
		{
			client.out_buf += "ERR_BADCHANMASK\r\n";
			continue;
		}

		// Create channel if it doesn't exist
		if (this->channels.find(chanName) == this->channels.end())
		{
			this->channels[chanName] = Channel(chanName);
		}

		Channel &channel = this->channels[chanName];

		// Check if client can join
		if (!channel.canJoin(client.getFd(), key))
		{
			if (channel.isMember(client.getFd()))
			{
				// Already in channel — silently skip
				continue;
			}
			else if (channel.isInviteOnly())
				client.out_buf += "ERR_INVITEONLYCHAN\r\n";
			else if (!channel.getKey().empty() && channel.getKey() != key)
				client.out_buf += "ERR_BADCHANNELKEY\r\n";
			else if (channel.getLimit() != 0 && channel.memberCount() >= channel.getLimit())
				client.out_buf += "ERR_CHANNELISFULL\r\n";
			else
				client.out_buf += "ERR_BADCHANMASK\r\n";
			continue;
		}

		// Add to channel
		channel.addMember(client.getFd());
		if (channel.memberCount() == 1)
			channel.addOperator(client.getFd());

		// Send JOIN message to all members in the channel (including the joiner)
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

		// RPL_TOPIC (if topic is set)
		if (!channel.getTopic().empty())
		{
			client.out_buf += ":server 332 " + client.nickname + " " + chanName + " :" + channel.getTopic() + "\r\n";
		}

		// RPL_NAMREPLY - list of users in the channel
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

		// RPL_ENDOFNAMES
		client.out_buf += ":server 366 " + client.nickname + " " + chanName + " :End of /NAMES list.\r\n";
	}
}
