#include "Channel.hpp"

Channel::Channel(std::string name)
{
	_name = name;
	_mode = "t";
	_limit = -1;
}

std::string&	Channel::getName(void)
{
	return _name;
}

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void	Channel::clearTopic(void)
{
	_topic.clear();
}

void	Channel::addClient(Client* client)
{
	if (_clientList.empty())
		_operatorList.insert(std::make_pair(client->getFd(), client));
	_clientList.insert(std::make_pair(client->getFd(), client));
}

std::string&	Channel::getTopic(void)
{
	return _topic;
}

void	Channel::removeClient(int clientFd)
{
	_clientList.erase(clientFd);
}

void	Channel::broadcast(std::string message)
{
	std::map<int, Client*>::iterator it;
	for (it = _clientList.begin(); it != _clientList.end(); ++it)
	{
		if (send(it->second->getFd(), message.c_str(), message.length(), 0) == -1)
			throw std::runtime_error("User not connected");
	}
}

std::string Channel::getKey(void)
{
	return _key;
}

void	Channel::setKey(std::string key)
{
	_key = key;
}

std::string	Channel::getMode(void)
{
	return _mode;
}

void	Channel::setMode(char mode)
{
	if (_mode.find(mode) == std::string::npos)
		_mode += mode;
}

void	Channel::removeMode(char mode)
{
	size_t pos = _mode.find(mode);
	if (pos != std::string::npos)
		_mode.erase(pos, 1);
}

int Channel::getLimit(void)
{
	return _limit;
}

void	Channel::setLimit(int limit)
{
	_limit = limit;
}

std::map<int, Client*>	Channel::getOperatorList(void)
{
	return _operatorList;
}

std::map<int, Client*>	Channel::getClientList(void)
{
	return _clientList;
}

void	Channel::setOperator(Client* client)
{
	_operatorList[client->getFd()] = client;
}

void	Channel::removeOperator(Client* client)
{
	_operatorList.erase(client->getFd());
}

int	Channel::checkClientExistByClientFd(int clientFd)
{
	std::map<int, Client*>::iterator it;
	for (it = _clientList.begin(); it != _clientList.end(); ++it)
	{
		if (it->second->getFd() == clientFd)
			return 1;
	}
	return 0;
}

int	Channel::isOperator(int clientFd)
{
	std::map<int, Client*>::iterator it;
	for (it = _operatorList.begin(); it != _operatorList.end(); ++it)
	{
		if (it->second->getFd() == clientFd)
			return 1;
	}
	return 0;
}

Client*	Channel::getClientByNickname(std::string nickName)
{
	std::map<int, Client*>::iterator it;
	for (it = _clientList.begin(); it != _clientList.end(); ++it)
	{
		if (it->second->getNickName() == nickName)
			return it->second;
	}
	return NULL;
}
