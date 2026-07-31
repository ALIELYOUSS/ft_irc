#include "../../includes/server.hpp"
#include <cctype>
static bool isMiddleType(const t_cmpnts &component)
{
	return component._type == MIDDLE;
}

bool server::valid_nick(const std::string &nick) const
{
	if (nick.empty())
		return false;
	if (!(std::isalpha(static_cast<unsigned char>(nick[0])) || nick[0] == '[' || nick[0] == ']' || nick[0] == '\\' || nick[0] == '`' || nick[0] == '_' || nick[0] == '^' || nick[0] == '{' || nick[0] == '|'))
		return false;
	for (size_t i = 1; i < nick.size(); ++i)
	{
		if (!(std::isalnum(static_cast<unsigned char>(nick[i])) || nick[i] == '-' || nick[i] == '_' || nick[i] == '[' || nick[i] == ']' || nick[i] == '\\' || nick[i] == '`' || nick[i] == '^' || nick[i] == '{' || nick[i] == '|' ))
			return false;
	}
	return true;
}

bool server::valid_username(const std::string &username) const
{
	if (username.empty())
		return false;
	for (size_t i = 0; i < username.size(); ++i)
	{
		if (username[i] == ' ' || username[i] == ':' || username[i] == '\r' || username[i] == '\n' || username[i] == '\0')
			return false;
	}
	return true;
}

bool server::nickname_coll(const std::string &nickname) const
{
	std::string lowerNick = tolow(nickname);
	for (size_t i = 0; i < this->client.size(); ++i)
	{
		if (tolow(this->client[i].nickname) == lowerNick)
			return true;
	}
	return false;
}

void server::cl_registration(Clients &client, const std::string &cmd)
{
	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;
	client.out_buf = "";
	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;
	if (cmpnt_index >= cmpnts.size() || cmpnts[cmpnt_index]._type != CMD || tolow(cmpnts[cmpnt_index]._data) != cmd)
		return;
	if (cmd == "pass")
	{
		if (client.password.size())
			client.out_buf = ":server 462 " + client.nickname + " :You may not reregister\r\n";
		else if (cmpnts.size() < cmpnt_index + 2)
			client.out_buf = ":server 461 " + client.nickname + " PASS :Not enough parameters\r\n";
		else if (!isMiddleType(cmpnts[cmpnt_index + 1]))
			client.out_buf = ":server 461 " + client.nickname + " PASS :Not enough parameters\r\n";
		else
			client.password = cmpnts[cmpnt_index + 1]._data;
	}
	else if (cmd == "nick")
	{
		if (cmpnts.size() < cmpnt_index + 2 || cmpnts[cmpnt_index + 1]._type != MIDDLE)
			client.out_buf = ":server 431 " + client.nickname + " :No nickname given\r\n";
		else if (!valid_nick(cmpnts[cmpnt_index + 1]._data))
			client.out_buf = ":server 432 " + client.nickname + " " + cmpnts[cmpnt_index + 1]._data + " :Erroneous nickname\r\n";
		else if (nickname_coll(cmpnts[cmpnt_index + 1]._data) && client.nickname != cmpnts[cmpnt_index + 1]._data)
			client.out_buf = ":server 433 " + client.nickname + " " + cmpnts[cmpnt_index + 1]._data + " :Nickname is already in use\r\n";
		else
		{
			std::string oldNick = client.nickname;
			client.nickname = cmpnts[cmpnt_index + 1]._data;
			if (client.registred && !oldNick.empty() && oldNick != client.nickname)
			{
				std::string nickMsg = ":" + oldNick + "!" + client.username + "@localhost NICK :" + client.nickname + "\r\n";
				for (std::map<std::string, Channel>::iterator chIt = this->channels.begin(); chIt != this->channels.end(); ++chIt)
				{
					Channel &ch = chIt->second;
					if (ch.isMember(client.getFd()))
					{
						const std::set<int> &members = ch.getMembers();
						for (std::set<int>::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
						{
							if (*mIt == client.getFd())
								continue;
							for (size_t j = 0; j < this->client.size(); ++j)
							{
								if (this->client[j].getFd() == *mIt)
								{
									this->client[j].out_buf += nickMsg;
									break;
								}
							}
						}
					}
				}
				client.out_buf += nickMsg;
			}
		}
	}
	else if (cmd == "user")
	{
		if (client.registred)
			client.out_buf = ":server 462 " + client.nickname + " :You may not reregister\r\n";
		else if (cmpnts.size() < cmpnt_index + 2)
			client.out_buf = ":server 461 " + client.nickname + " USER :Not enough parameters\r\n";
		else if (cmpnts[cmpnt_index]._type != CMD || cmpnts[cmpnt_index + 1]._type != MIDDLE)
			client.out_buf = ":server 461 " + client.nickname + " USER :Not enough parameters\r\n";
		else if (!valid_username(cmpnts[cmpnt_index + 1]._data))
			client.out_buf = ":server 468 " + client.nickname + " :Invalid username\r\n";
		else
			client.username = cmpnts[cmpnt_index + 1]._data;
	}
	if (!client.password.empty() && client.password != this->passwd)
	{
		client.password.clear();
		client.out_buf = ":server 464 " + client.nickname + " :Password incorrect\r\n";
		return;
	}
	if (client.out_buf.empty() && !client.registred && !client.nickname.empty() && !client.password.empty() && !client.username.empty())
	{
		client.out_buf = ":server 001 " + client.nickname + " :Welcome to the Internet Relay Network " + client.nickname + "!" + client.username + "@localhost\r\n";
		client.registred = true;
	}
}
