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

void	Channel::broadcast(std::string message, Client* client)
{
	std::map<int, Client*>::iterator it;
	for (it = _clientList.begin(); it != _clientList.end(); ++it)
	{
		if (it->second == client)
			continue ;
		it->second->addWriteBuff(message);
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
