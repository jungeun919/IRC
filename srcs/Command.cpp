#include "Command.hpp"

void	Command::runCommand(std::vector<std::string> token, Server *server, Client *client)
{
	if (token[1] == "PASS") {
		if (token.size() != 3)
			throw std::runtime_error("Invalid argument");
		pass(server, client, token[2]);
	}
	else if (token[1] == "NICK") {
		if (token.size() != 3)
			throw std::runtime_error("Invalid argument");
		nick(server, client, token[2]);
	}
	else if (token[1] == "USER") {
		if (token.size() != 6)
			throw std::runtime_error("Invalid argument");
		if (token[3] != "0" || token[4] != "*" || token[5][0] != ':')
			throw std::runtime_error("Invalid argument");
		user(server, client, token[2], token[5].substr(1));
	}
	else if (token[1] == "JOIN") {
		if (token.size() != 3 && token.size() != 4)
			throw std::runtime_error("Invalid argument");
		if (token[2][0] != '#')
			throw std::runtime_error("Invalid argument");
		join(server, client, token);
	}
	else if (token[1] == "PRIVMSG") {
		if (token.size() != 4)
			throw std::runtime_error("Invalid argument");
		privmsg(server, client, token[2], token[3]);
	}
}

void	Command::pass(Server *server, Client *client, std::string password)
{	
	if (client->getAuthorized() >= 1)
		throw std::runtime_error("Already authorized");
	if (server->getPassword() == password)
		client->setAuthorized(1);
	else
		throw std::runtime_error("Wrong password");
}

void	Command::nick(Server *server, Client *client, std::string nickName)
{
	if (!client->getAuthorized())
		throw std::runtime_error("Not authorized");
	
	if (server->checkNickName(nickName))
		throw std::runtime_error("NickName already exist");
	
	client->setNickName(nickName);
}

void	Command::user(Server *server, Client *client, std::string userName, std::string realName)
{
	if (!client->getAuthorized())
		throw std::runtime_error("Not authorized");
	
	if (server->checkUserName(userName))
		throw std::runtime_error("UserName already exist");

	if (server->checkRealName(realName))
		throw std::runtime_error("RealName already exist");
	client->setUserName(userName);
	client->setRealName(realName);
}

void	Command::join(Server *server, Client *client, std::vector<std::string> token)
{
	if (!client->getAuthorized())
		throw std::runtime_error("Not authorized");
	
	std::string channelName = token[2].substr(1);
	std::string key = "";

	if (token.size() == 4)
		key = token[3];
	
	if (!server->checkChannelName(channelName))
	{
		server->addChannel(channelName);
		server->_channelList[channelName]->addClient(client);
		client->addChannel(channelName, server->_channelList[channelName]);
		server->_channelList[channelName]->setKey(key);
	}
	else
	{
		if (server->_channelList[channelName]->getKey() == key)
		{
			server->_channelList[channelName]->addClient(client);
			client->addChannel(channelName, server->_channelList[channelName]);
		}
		else
			throw std::runtime_error("Wrong key");
	}
}

void	Command::privmsg(Server *server, Client *client, std::string target, std::string message)
{
	if (!client->getAuthorized())
		throw std::runtime_error("Not authorized");
	message = client->getNickName() + ": " + message + "\n";
	if (target[0] == '#')
	{
		target = target.substr(1);
		if (!server->checkChannelName(target))
			throw std::runtime_error("Channel doesn't exist");
		
		std::map<int, Client*> clientList = server->_channelList[target]->_clientList;
		std::map<int, Client*>::iterator it = clientList.begin();
		while (it != clientList.end())
		{
			if (!send((*it).second->getFd(), message.c_str(), message.length(), 0))
				throw std::runtime_error("User not connected");
			it++;
		}
	}
	else
	{
		if (!server->checkNickName(target))
			throw std::runtime_error("NickName doesn't exist");
		if (!send(server->getFdByNickName(target), message.c_str(), message.length(), 0))
			throw std::runtime_error("User not connected");
	}
}