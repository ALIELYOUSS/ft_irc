#include "../../includes/server.hpp"

#include <cctype>

/*
 * MODE <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>]
 *
 * Implemented modes:
 *   i - Set/remove Invite-only channel
 *   t - Set/remove TOPIC restriction to channel operators
 *   k - Set/remove the channel key (password)
 *   o - Give/take channel operator privilege
 *   l - Set/remove the user limit to channel
 */
void server::cl_mode(Clients &client)
{
	if (!client.registred)
	{
		client.out_buf += ":server 451 " + client.nickname + " MODE :You have not registered\r\n";
		return;
	}

	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;

	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;

	if (cmpnts.size() < cmpnt_index + 2 || cmpnts[cmpnt_index + 1]._type != MIDDLE)
	{
		client.out_buf += ":server 461 " + client.nickname + " MODE :Not enough parameters\r\n";
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
	if (cmpnts.size() < cmpnt_index + 3)
	{
		std::string modeStr = "+";
		std::string modeParams;

		if (channel.isInviteOnly())
			modeStr += "i";
		if (channel.isTopicRestricted())
			modeStr += "t";
		if (!channel.getKey().empty())
		{
			modeStr += "k";
			modeParams += " " + channel.getKey();
		}
		if (channel.getLimit() > 0)
		{
			modeStr += "l";
			std::ostringstream oss;
			oss << channel.getLimit();
			modeParams += " " + oss.str();
		}

		client.out_buf += ":server 324 " + client.nickname + " " + chanName + " " + modeStr + "\r\n";
		client.out_buf += ":server 329 " + client.nickname + " " + chanName + " :Channel created\r\n";
		return;
	}
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
	std::string modeStr = cmpnts[cmpnt_index + 2]._data;
	if (modeStr.empty())
		return;

	std::vector<std::string> modeArgs;
	for (size_t i = cmpnt_index + 3; i < cmpnts.size(); ++i)
	{
		const t_cmpnts &comp = cmpnts[i];
		if (comp._type == MIDDLE || comp._type == TRAILING)
			modeArgs.push_back(comp._data);
	}
	size_t argIdx = 0;

	std::string changeMsg;
	std::vector<std::string> changeParams;

	char currentSign = '+';
	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char c = modeStr[i];

		if (c == '+')
		{
			currentSign = '+';
			continue;
		}
		if (c == '-')
		{
			currentSign = '-';
			continue;
		}
		if (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l')
		{
			client.out_buf += ":server 472 " + client.nickname + " " + c + " :is unknown mode char to me\r\n";
			continue;
		}
		bool adding = (currentSign == '+');
		switch (c)
		{
		case 'i':
		{
			if (channel.isInviteOnly() == adding)
				continue;
			channel.setInviteOnly(adding);
			changeMsg += c;
			break;
		}
		case 't':
		{
			if (channel.isTopicRestricted() == adding)
				continue;
			channel.setTopicRestricted(adding);
			changeMsg += c;
			break;
		}
		case 'k':
		{
			if (adding)
			{
				if (argIdx >= modeArgs.size())
				{
					client.out_buf += ":server 461 " + client.nickname + " MODE :Not enough parameters\r\n";
					continue;
				}
				std::string newKey = modeArgs[argIdx++];
				channel.setKey(newKey);
				changeMsg += c;
			}
			else
			{
				channel.setKey("");
				changeMsg += c;
			}
			break;
		}
		case 'l':
		{
			if (adding)
			{
				if (argIdx >= modeArgs.size())
				{
					client.out_buf += ":server 461 " + client.nickname + " MODE :Not enough parameters\r\n";
					continue;
				}
				std::string limitStr = modeArgs[argIdx++];
				bool valid = true;
				long limitVal = 0;
				for (size_t j = 0; j < limitStr.size(); ++j)
				{
					if (!std::isdigit(static_cast<unsigned char>(limitStr[j])))
					{
						valid = false;
						break;
					}
					limitVal = limitVal * 10 + (limitStr[j] - '0');
					if (limitVal > 65535)
					{
						valid = false;
						break;
					}
				}
				if (!valid || limitVal < 0)
				{
					client.out_buf += ":server 696 " + client.nickname + " " + chanName + " l :Invalid limit value\r\n";
					continue;
				}
				channel.setLimit(static_cast<size_t>(limitVal));
				changeMsg += c;
			}
			else
			{
				channel.setLimit(0);
				changeMsg += c;
			}
			break;
		}
		case 'o':
		{
			if (argIdx >= modeArgs.size())
			{
				client.out_buf += ":server 461 " + client.nickname + " MODE :Not enough parameters\r\n";
				continue;
			}
			std::string targetNick = modeArgs[argIdx++];

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

			if (adding)
			{
				if (channel.isOperator(targetFd))
					continue;
				channel.addOperator(targetFd);
			}
			else
			{
				if (!channel.isOperator(targetFd))
					continue;
				channel.removeOperator(targetFd);
			}
			changeMsg += c;
			break;
		}
		default:
			break;
		}
	}

	if (changeMsg.empty())
		return;

	std::string broadcastMode = "";
	currentSign = '\0';
	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char c = modeStr[i];
		if (c == '+' || c == '-')
		{
			if (broadcastMode.empty() || broadcastMode[broadcastMode.size()-1] != c)
				broadcastMode += c;
			currentSign = c;
		}
		else
		{
			broadcastMode += c;
		}
	}

	std::string modeBroadcast = ":" + client.nickname + "!" + client.username + "@localhost MODE " + chanName + " " + broadcastMode + "\r\n";
	const std::set<int> &members = channel.getMembers();
	for (std::set<int>::iterator it2 = members.begin(); it2 != members.end(); ++it2)
	{
		int memberFd = *it2;
		for (size_t j = 0; j < this->client.size(); ++j)
		{
			if (this->client[j].getFd() == memberFd)
			{
				this->client[j].out_buf += modeBroadcast;
				break;
			}
		}
	}
}
