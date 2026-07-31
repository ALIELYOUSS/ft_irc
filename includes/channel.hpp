#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <set>
#include <string>
class Channel
{
private:
    std::string name;
    std::string topic;
    std::string key;
    size_t limit;
    bool inviteOnly;
    bool topicRestricted;
    std::set<int> members;
    std::set<int> operators;
    std::set<int> invited;
public:
    Channel();
    explicit Channel(const std::string &channelName);
    ~Channel();
    const std::string &getName() const;
    void setName(const std::string &channelName);
    const std::string &getTopic() const;
    void setTopic(const std::string &channelTopic);
    const std::string &getKey() const;
    void setKey(const std::string &channelKey);
    size_t getLimit() const;
    void setLimit(size_t newLimit);
    bool isInviteOnly() const;
    void setInviteOnly(bool enabled);
    bool isTopicRestricted() const;
    void setTopicRestricted(bool enabled);
bool isMember(int clientFd) const;
    bool isOperator(int clientFd) const;
    bool isInvited(int clientFd) const;
    const std::set<int> &getMembers() const;
    size_t memberCount() const;
    bool empty() const;
    bool canJoin(int clientFd, const std::string &joinKey) const;
    bool addMember(int clientFd);
    bool removeMember(int clientFd);
    bool addOperator(int clientFd);
    bool removeOperator(int clientFd);
    void invite(int clientFd);
    void uninvite(int clientFd);
};
#endif
