#include "../../includes/server.hpp"
void server::cl_invite(Clients &client)
{
	if (!client.registred)
	{
		client.out_buf += ":server 451 " + client.nickname + " :You have not registered\r\n";
		return;
	}
	const std::vector<t_cmpnts> &cmpnts = client.getComponents();
	size_t cmpnt_index = 0;
	if (!cmpnts.empty() && cmpnts[0]._type == PREFIX)
		cmpnt_index = 1;
	if (cmpnts.size() < cmpnt_index + 3 || cmpnts[cmpnt_index + 1]._type != MIDDLE || cmpnts[cmpnt_index + 2]._type != MIDDLE)
	{
		client.out_buf += ":server 461 " + client.nickname + " INVITE :Not enough parameters\r\n";
		return;
	}
	std::string targetNick = cmpnts[cmpnt_index + 1]._data;
	std::string chanName = cmpnts[cmpnt_index + 2]._data;
	if (chanName.empty() || chanName[0] != '#')
	{
		client.out_buf += ":server 403 " + client.nickname + " " + chanName + " :No such channel\r\n";
		return;
	}
	int targetFd = -1;
	for (size_t i = 0; i < this->client.size(); ++i)
	{
		if (this->client[i].nickname == targetNick)
		{
			targetFd = this->client[i].getFd();
			break;
		}
	}
	if (targetFd == -1)
	{
		client.out_buf += ":server 401 " + client.nickname + " " + targetNick + " :No such nick/channel\r\n";
		return;
	}
	std::map<std::string, Channel>::iterator it = this->channels.find(chanName);
	if (it == this->channels.end())
	{
		client.out_buf += ":server 403 " + client.nickname + " " + chanName + " :No such channel\r\n";
		return;
	}
	Channel &channel = it->second;
	if (channel.isInviteOnly())
	{
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
	}
	if (channel.isMember(targetFd))
	{
		client.out_buf += ":server 443 " + client.nickname + " " + targetNick + " " + chanName + " :is already on channel\r\n";
		return;
	}
	channel.invite(targetFd);
	client.out_buf += ":server 341 " + client.nickname + " " + chanName + " " + targetNick + "\r\n";
	for (size_t i = 0; i < this->client.size(); ++i)
	{
		if (this->client[i].getFd() == targetFd)
		{
			this->client[i].out_buf += ":" + client.nickname + "!" + client.username + "@localhost INVITE " + targetNick + " " + chanName + "\r\n";
			break;
		}
	}
}
