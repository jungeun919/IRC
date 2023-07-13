#include "Channel.hpp"

Channel::Channel(std::string name)
{
	_name = name;
}

std::string&	Channel::getName(void)
{
	return _name;
}

std::map<int, Client*>&	Channel::getClientList(void)
{
	return _clientList;
}

void	Channel::addClient(Client* client)
{
	if (_clientList.empty())
		_operatorList.insert(std::make_pair(client->getFd(), client));
	_clientList.insert(std::make_pair(client->getFd(), client));
}

void	Channel::removeClient(int clientFd)
{
	_clientList.erase(clientFd);
}

void	Channel::broadcast(std::string message)
{
	std::map<int, Client*>::iterator it;
	for (it = _clientList.begin(); it != _clientList.end(); ++it)
		it->second->addWriteBuff(message);
}

std::string Channel::getKey(void)
{
	return _key;
}

void	Channel::setKey(std::string key)
{
	_key = key;
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
