#include "../../includes/server.hpp"
void server::cl_topic(Clients &client)
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
	if (cmpnts.size() < cmpnt_index + 2 || cmpnts[cmpnt_index + 1]._type != MIDDLE)
	{
		client.out_buf += ":server 461 " + client.nickname + " TOPIC :Not enough parameters\r\n";
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
	bool hasTopic = false;
	std::string newTopic;
	size_t last_idx = cmpnts.size() - 1;
	if (last_idx > cmpnt_index + 1 && cmpnts[last_idx]._type == TRAILING)
	{
		hasTopic = true;
		newTopic = cmpnts[last_idx]._data;
	}
	else if (cmpnts.size() >= cmpnt_index + 3 && cmpnts[cmpnt_index + 2]._type == MIDDLE)
	{
		hasTopic = true;
		newTopic = cmpnts[cmpnt_index + 2]._data;
	}
	if (!hasTopic)
	{
		if (channel.getTopic().empty())
			client.out_buf += ":server 331 " + client.nickname + " " + chanName + " :No topic is set\r\n";
		else
			client.out_buf += ":server 332 " + client.nickname + " " + chanName + " :" + channel.getTopic() + "\r\n";
		return;
	}
	if (channel.isTopicRestricted() && !channel.isOperator(client.getFd()))
	{
		client.out_buf += ":server 482 " + client.nickname + " " + chanName + " :You're not channel operator\r\n";
		return;
	}
	channel.setTopic(newTopic);
	std::string topicMsg = ":" + client.nickname + "!" + client.username + "@localhost TOPIC " + chanName + " :" + newTopic + "\r\n";
	const std::set<int> &members = channel.getMembers();
	for (std::set<int>::iterator it2 = members.begin(); it2 != members.end(); ++it2)
	{
		int memberFd = *it2;
		for (size_t j = 0; j < this->client.size(); ++j)
		{
			if (this->client[j].getFd() == memberFd)
			{
				this->client[j].out_buf += topicMsg;
				break;
			}
		}
	}
}
