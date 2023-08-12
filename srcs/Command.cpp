#include "Command.hpp"

void	Command::runCommand(std::vector<std::string> token, Server *server, Client *client)
{
	if (token[1] == "PASS")
		pass(server, client, token);
	else if (token[1] == "NICK")
		nick(server, client, token);
	else if (token[1] == "USER")
		user(server, client, token);
	else if (token[1] == "JOIN")
		join(server, client, token);
	else if (token[1] == "PRIVMSG")
		privmsg(server, client, token);
	else if (token[1] == "KICK")
		kick(server, client, token);
	else if (token[1] == "INVITE")
		invite(server, client, token);
	else if (token[1] == "TOPIC")
		topic(server, client, token);
	else if (token[1] == "MODE")
		mode(server, client, token);
}

void	Command::pass(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 3)
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() >= 1)
		throw std::runtime_error("Already authorized");
	
	std::string password = token[2];
	if (server->getPassword() == password)
	{
		client->setAuthorized(1);
		server->noticeToClient(client->getFd(), "Enter NickName  ex)NICK <nickname>");
		std::cout << "Client" << client->getFd() << " Login Success" << std::endl;
	}
	else
		throw std::runtime_error("Wrong password");
}

void	Command::nick(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 3)
		throw std::runtime_error("Invalid argument");
	if (!client->getAuthorized())
		throw std::runtime_error("Not authorized");
	
	std::string nickName = token[2];
	if (server->checkNickName(nickName))
		throw std::runtime_error("NickName already exist");
	
	if (client->getAuthorized() == 1)
	{
		client->setAuthorized(2);
		server->noticeToClient(client->getFd(), "Enter UserName  ex)USER <username> 0 * :<realname>");
	}
	client->setNickName(nickName);
	std::cout << "Client" << client->getFd() << " NickName is " << nickName << std::endl;
}

void	Command::user(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 6 || token[3] != "0" || token[4] != "*" || token[5][0] != ':')
		throw std::runtime_error("Invalid argument");
	
	if (client->getAuthorized() < 2)
		throw std::runtime_error("Not authorized");
	if (client->getAuthorized() >= 3)
		throw std::runtime_error("Already set UserName and RealName");

	std::string userName = token[2];
	std::string realName = token[5].substr(1);
	if (server->checkUserName(userName))
		throw std::runtime_error("UserName already exist");
	if (server->checkRealName(realName))
		throw std::runtime_error("RealName already exist");

	client->setAuthorized(3);
	client->setUserName(userName);
	client->setRealName(realName);
	std::cout << "Client" << client->getFd() << " UserName is " << userName << std::endl;
	std::cout << "Client" << client->getFd() << " RealName is " << realName << std::endl;
	server->noticeToClient(client->getFd(), "Welcome to the Internet Relay Network " + client->getNickName() + "!" + client->getUserName() + "@irc.42.fr");
}

void	Command::join(Server *server, Client *client, std::vector<std::string> token)
{
	if ((token.size() != 3 && token.size() != 4) || token[2][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	std::vector<std::string> channelName = split(token[2], ",");
	std::vector<std::string> key;
	if (!token[3].empty())
		key = split(token[3], ",");

	while (channelName.size() > key.size())
		key.push_back("");

	for (size_t i = 0; i < channelName.size(); i++)
	{
		try {
			channelName[i] = channelName[i].substr(1);
			if (!server->checkChannelName(channelName[i]))
			{
				server->addChannel(channelName[i]);
				Channel *channel = server->getChannelByChannelName(channelName[i]);
				if (token.size() == 4)
					channel->setKey(key[i]);
				else
					channel->setKey("");
				if (channel->getLimit() == -1 || channel->getLimit() > static_cast<int>(channel->getClientList().size()))
				{
					channel->addClient(client);
					client->addChannel(channelName[i], channel);
					server->noticeToClient(client->getFd(), "Channel #" + channelName[i] + " created");
				}
				else
					throw std::runtime_error("Exceed user limit");
			}
			else
			{
				Channel *channel = server->getChannelByChannelName(channelName[i]);
				if (channel == NULL)
					throw std::runtime_error("Channel doesn't exist");
				if (channel->getMode().find('i') != std::string::npos)
					throw std::runtime_error("Channel is set to INVITE-ONLY mode");
				
				if ((token.size() == 4) ? channel->getKey() == key[i]: true)
				{
					if (channel->checkClientExistByClientFd(client->getFd()))
						throw std::runtime_error("Already joined channel");
					if (channel->getLimit() == -1 || channel->getLimit() > static_cast<int>(channel->getClientList().size()))
					{
						std::map<int, Client*> clientList = channel->getClientList();
						for (std::map<int, Client*>::iterator it = clientList.begin(); it != clientList.end(); it++)
							server->noticeToClient(it->first, client->getNickName() + " has joined Channel #" + channelName[i]);

						channel->addClient(client);
						client->addChannel(channelName[i], channel);
						server->noticeToClient(client->getFd(), "Channel #" + channelName[i] + " joined");
					}
					else
						throw std::runtime_error("Exceed user limit");
				}
				else
					throw std::runtime_error("Wrong key");
			}
		}
		catch (std::exception &e) {
			std::string msg = "Channel #" + channelName[i] + " " + e.what();
			std::cout << "Client" << client->getFd() << " " << msg << std::endl;
			server->noticeToClient(client->getFd(), msg);
		}
	}
}

void	Command::privmsg(Server *server, Client *client, std::vector<std::string> token)
{	
	if (token.size() < 4 || token[3][0] != ':')
			throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	token[3] = token[3].substr(1);
	std::string message = client->getNickName() + ": ";
	
	int i = 3;
	while (i < static_cast<int>(token.size()))
	{
		message += token[i];
		message += " ";
		i++;
	}
	message += "\n";

	std::string target = token[2];
	if (target[0] == '#')
	{
		target = target.substr(1);
		Channel *channel = server->getChannelByChannelName(target);
		if (channel == NULL)
			throw std::runtime_error("Channel doesn't exist");
		if (channel->checkClientExistByClientFd(client->getFd()) == 0)
			throw std::runtime_error("User is not in channel");
		
		std::map<int, Client*> clientList = server->getChannelByChannelName(target)->getClientList();
		std::map<int, Client*>::iterator it = clientList.begin();
		while (it != clientList.end())
		{
			it->second->addWriteBuff(message);
			it++;
		}
	}
	else
	{
		if (!server->checkNickName(target) || server->getClientByNickname(target)->getAuthorized() < 3)
			throw std::runtime_error("User doesn't exist");
		
		Client *client = server->getClientByNickname(target);
		client->addWriteBuff(message);
	}
}

void	Command::kick(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 4 && token.size() != 5)
		throw std::runtime_error("Invalid argument");
	if (token[2][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (token.size() == 5)
	{
		if (token[4][0] != ':')
			throw std::runtime_error("Invalid argument");
	}
	
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	// 채널명 있는지 확인
	Channel *channel = server->getChannelByChannelName(token[2].substr(1));
	if (channel == NULL)
		throw std::runtime_error("Channel doesn't exist");
	
	// 입력 유저가 channel에 있는지, operator인지 확인
	if (channel->checkClientExistByClientFd(client->getFd()) == 0)
		throw std::runtime_error("User is not in channel");
	if (channel->isOperator(client->getFd()) == 0)
		throw std::runtime_error("User is not operator");
	
	// kickClients (token[3] : nickname list)
	std::vector<std::string> kickClients = split(token[3], ",");
	std::vector<std::string>::iterator it;

	for (it = kickClients.begin(); it != kickClients.end(); ++it)
	{
		Client* kickClient = channel->getClientByNickname(*it);

		if (channel->checkClientExistByClientFd(server->getFdByNickName(*it)) == 0) {
			std::string msg = *it + " is not in channel";
			server->noticeToClient(client->getFd(), msg);
			continue;
		}
		if (!kickClient)
			continue ;
		if (kickClient == client) {
			server->noticeToClient(client->getFd(), "You can't kick yourself");
			continue ;
		}
		channel->removeClient(kickClient->getFd());
		server->noticeToClient(kickClient->getFd(), "You are kicked from " + token[2]);
		kickClient->removeChannel(token[2].substr(1));

		std::cout << "kick " << kickClient->getFd() << " from " << token[2] << std::endl;
		
		std::stringstream ss;
		ss << "Kick " << kickClient->getFd() << " from " << token[2] << "\n";
		std::string kickMessage = ss.str();
		channel->broadcast(kickMessage);

		std::string message = "There is no reason\n";
		if (token.size() == 5)
			message = "Client" + client->getNickName() + " kicked because " + token[4].substr(1) + '\n';	
		channel->broadcast(message);
	}
}

void	Command::invite(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 4 || token[3][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	std::string nickName = token[2];
	std::string channelName = token[3].substr(1);
	// 채널명 있는지 확인
	Channel *channel = server->getChannelByChannelName(channelName);
	if (channel == NULL)
		throw std::runtime_error("Channel doesn't exist");
	
	// 입력 유저가 channel에 있는지, operator인지 확인
	if (channel->checkClientExistByClientFd(client->getFd()) == 0)
		throw std::runtime_error("User is not in channel");
	if (channel->isOperator(client->getFd()) == 0)
		throw std::runtime_error("User is not operator");
	if (channel->checkClientExistByClientFd(server->getFdByNickName(nickName)) == 1)
		throw std::runtime_error("User is already in channel");
	
	Client* inviteClient = server->getClientByNickname(nickName);
	if (!inviteClient)
		return ;
	channel->addClient(inviteClient);
	channel->broadcast("Invite " + inviteClient->getNickName() + " to " + channelName + "\n");
	std::cout << "channel #" << channelName << " client count: " << channel->getClientList().size() << std::endl;
}

void	Command::topic(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() < 3 || token[2][0] != '#')
		throw std::runtime_error("Invalid argument");
	
	// 채널명 있는지 확인
	Channel *channel = server->getChannelByChannelName(token[2].substr(1));
	if (channel == NULL)
		throw std::runtime_error("Channel doesn't exist");
	
	// 입력 유저가 channel에 있는지, operator인지, channel mode에 't' 있는지 확인
	if (channel->checkClientExistByClientFd(client->getFd()) == 0)
		throw std::runtime_error("User is not in channel");
	if (channel->getMode().find('t') != std::string::npos)
	{
		if (channel->isOperator(client->getFd()) == 0)
			throw std::runtime_error("User is not operator");
	}

	if (token.size() == 3)
	{
		std::string message = channel->getTopic() + "\n";
		client->addWriteBuff(message);
	}
	else
	{
		if (client->getAuthorized() < 3)
			throw std::runtime_error("Not authorized");
		if (token[3][0] != ':')
			throw std::runtime_error("Invalid argument");
		std::string topic = "";
		token[3] = token[3].substr(1);

		int i = 3;
		while (!token[i].empty())
		{
			topic += token[i];
			topic += " ";
			i++;
		}

		if (topic.empty())
			channel->clearTopic();
		else
			channel->setTopic(topic);
	}
}

void Command::mode(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 4 && token.size() != 5)
		throw std::runtime_error("Invalid argument");
	if (token[2][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (token[3][0] != '+' && token[3][0] != '-')
		throw std::runtime_error("Invalid argument");
	if (token[3][1] != 'i' && token[3][1] != 't' && token[3][1] != 'k' && token[3][1] != 'o' && token[3][1] != 'l')
		throw std::runtime_error("Invalid argument");
	if (token[3].length() != 2)
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	// 채널명 있는지 확인
	Channel *channel = server->getChannelByChannelName(token[2].substr(1));
	if (channel == NULL)
		throw std::runtime_error("Channel doesn't exist");
	
	// 입력 유저가 channel에 있는지, operator인지 확인
	if (channel->isOperator(client->getFd()) == 0)
		throw std::runtime_error("User is not operator");
	if (token[3][1] == 'o')
		if (channel->getClientByNickname(token[4]) == NULL)
			throw std::runtime_error("User is not in channel");
	

	if (token[3][1] == 'i')
	{
		if (token[3][0] == '+')
			channel->setMode('i');
		else
			channel->removeMode('i');
	}
	else if (token[3][1] == 't')
	{
		if (token[3][0] == '+')
			channel->setMode('t');
		else
			channel->removeMode('t');
	}
	if (token[3][1] == 'k')
	{
		if (token[3][0] == '+')
			channel->setKey(token[4]);
		else
		{
			if (token.size() != 4)
				throw std::runtime_error("Invalid argument");
			channel->setKey("");
		}
	}
	else if (token[3][1] == 'o')
	{
		Client *opClient = channel->getClientByNickname(token[4]);
		if (token[3][0] == '+')
			channel->setOperator(opClient);
		else
			channel->removeOperator(opClient);
	}
	else if (token[3][1] == 'l')
	{
		if (token[3][0] == '+')
		{
			channel->setMode('l');
			int limit = atoi(token[4].c_str());
			if (limit < 0)
				return;
			channel->setLimit(limit);
		}
		else
		{
			if (token.size() != 4)
				throw std::runtime_error("Invalid argument");
			channel->removeMode('l');
			channel->setLimit(-1);
		}
	}
}

std::vector<std::string>	Command::split(std::string str, std::string delimiter) {
	std::vector<std::string> ret;

	size_t pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		ret.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}
	if (!str.empty())
		ret.push_back(str);
	return ret;
}
