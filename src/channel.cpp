#include "../includes/channel.hpp"


static bool isValidChannelName(const std::string &channelName){
    if (channelName.empty() || channelName[0] != '#')
        return false;
    for (size_t i = 1; i < channelName.size(); ++i)
    {
        if (channelName[i] == ' ' || channelName[i] == ',' || channelName[i] == '\a' || channelName[i] == '\r' || channelName[i] == '\n')
            return false;
    }
    return true;
}

Channel::Channel() : name(""), topic(""), key(""), limit(0), inviteOnly(false), topicRestricted(false){}

Channel::Channel(const std::string &channelName) : name(channelName), topic(""), key(""), limit(0), inviteOnly(false), topicRestricted(false){}

Channel::~Channel(){}

const std::string &Channel::getName() const{
	return this->name;
}

void Channel::setName(const std::string &channelName){
	this->name = channelName;
}

const std::string &Channel::getTopic() const{
	return this->topic;
}

void Channel::setTopic(const std::string &channelTopic){
	this->topic = channelTopic;
}

const std::string &Channel::getKey() const{
	return this->key;
}

void Channel::setKey(const std::string &channelKey){
	this->key = channelKey;
}

size_t Channel::getLimit() const{
	return this->limit;
}

void Channel::setLimit(size_t newLimit){
	this->limit = newLimit;
}

bool Channel::isInviteOnly() const{
	return this->inviteOnly;
}

void Channel::setInviteOnly(bool enabled){
	this->inviteOnly = enabled;
}

bool Channel::isTopicRestricted() const{
	return this->topicRestricted;
}

void Channel::setTopicRestricted(bool enabled){
	this->topicRestricted = enabled;
}

bool Channel::isMember(int clientFd) const{
	return this->members.find(clientFd) != this->members.end();
}

bool Channel::isOperator(int clientFd) const{
	return this->operators.find(clientFd) != this->operators.end();
}

bool Channel::isInvited(int clientFd) const{
	return this->invited.find(clientFd) != this->invited.end();
}

const std::set<int> &Channel::getMembers() const
{
	return this->members;
}

size_t Channel::memberCount() const{
	return this->members.size();
}

bool Channel::empty() const{
	return this->members.empty();
}

bool Channel::canJoin(int clientFd, const std::string &joinKey) const{
	if (clientFd < 0)
		return false;
	if (!isValidChannelName(this->name))
		return false;
	if (isMember(clientFd))
		return false;
	if (this->inviteOnly && !isInvited(clientFd))
		return false;
	if (!this->key.empty() && this->key != joinKey)
		return false;
	if (this->limit != 0 && this->members.size() >= this->limit)
		return false;
	return true;
}

bool Channel::addMember(int clientFd){
	if (clientFd < 0)
		return false;
	return this->members.insert(clientFd).second;
}

bool Channel::removeMember(int clientFd){
	return this->members.erase(clientFd) > 0;
}

bool Channel::addOperator(int clientFd){
	if (clientFd < 0)
		return false;
	return this->operators.insert(clientFd).second;
}

bool Channel::removeOperator(int clientFd){
	return this->operators.erase(clientFd) > 0;
}

void Channel::invite(int clientFd){
	if (clientFd >= 0)
		this->invited.insert(clientFd);
}

void Channel::uninvite(int clientFd){
	this->invited.erase(clientFd);
}
