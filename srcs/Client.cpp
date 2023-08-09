#include "Client.hpp"

Client::Client(int fd)
{
	_fd = fd;
	_authorized = 0;
}

Client::~Client(void)
{
	close(_fd);
}

int	Client::getFd(void)
{
	return _fd;
}

int Client::getAuthorized(void)
{
	return _authorized;
}

void Client::setAuthorized(int authorized)
{
	_authorized = authorized;
}

std::string& Client::getUserName(void)
{
	return _userName;
}

void Client::setUserName(std::string userName)
{
	_userName = userName;
}

std::string& Client::getRealName(void)
{
	return _realName;
}

void Client::setRealName(std::string realName)
{
	_realName = realName;
}

std::string& Client::getNickName(void)
{
	return _nickName;
}

void Client::setNickName(std::string nickName)
{
	_nickName = nickName;
}

std::map<std::string, Channel*>	Client::getChannelList(void)
{
	return _channelList;
}

void Client::addChannel(std::string channelName, Channel* channel)
{
	_channelList.insert(std::pair<std::string, Channel*>(channelName, channel));
}

void Client::removeChannel(std::string channelName)
{
	std::map<std::string, Channel*>::iterator it = _channelList.find(channelName);
	if (it == _channelList.end())
		return ;
	_channelList.erase(it);
}
