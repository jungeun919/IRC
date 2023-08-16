#include "Command.hpp"

void	Command::runCommand(std::vector<std::string> token, Server *server, Client *client)
{
	if (token[0] == "PASS")
		pass(server, client, token);
	else if (token[0] == "NICK")
		nick(server, client, token);
	else if (token[0] == "USER")
		user(server, client, token);
	else if (token[0] == "JOIN")
		join(server, client, token);
	else if (token[0] == "PRIVMSG")
		privmsg(server, client, token);
	else if (token[0] == "KICK")
		kick(server, client, token);
	else if (token[0] == "INVITE")
		invite(server, client, token);
	else if (token[0] == "TOPIC")
		topic(server, client, token);
	else if (token[0] == "MODE")
		mode(server, client, token);
}

void	Command::pass(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 2)
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() >= 1)
		throw std::runtime_error("Already authorized");
	
	std::string password = token[1];
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
	if (token.size() != 2)
		throw std::runtime_error("Invalid argument");
	if (!client->getAuthorized())
		throw std::runtime_error("Not authorized");
	
	std::string nickName = token[1];
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
	if (token.size() != 5 || token[2] != "0" || token[3] != "*" || token[4][0] != ':')
		throw std::runtime_error("Invalid argument");
	
	if (client->getAuthorized() < 2)
		throw std::runtime_error("Not authorized");
	if (client->getAuthorized() >= 3)
		throw std::runtime_error("Already set UserName and RealName");

	std::string userName = token[1];
	std::string realName = token[4].substr(1);
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
	if ((token.size() != 2 && token.size() != 3) || token[1][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	std::vector<std::string> channelName = split(token[1], ',');
	std::vector<std::string> key;
	if (!token[2].empty())
		key = Command::key_split(token[2], ',');

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
				if (token.size() == 3)
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
				
				if ((token.size() == 3) ? channel->getKey() == key[i]: channel->getKey() == "")
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
	if (token.size() < 3 || token[2][0] != ':')
			throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	token[2] = token[2].substr(1);
	std::string message = client->getNickName() + ": ";
	
	size_t i = 2;
	while (i < token.size())
	{
		message += token[i];
		message += " ";
		i++;
	}
	message += "\n";

	std::string target = token[1];
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
	if (token.size() != 3 && token.size() != 4)
		throw std::runtime_error("Invalid argument");
	if (token[1][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (token.size() == 4)
	{
		if (token[3][0] != ':')
			throw std::runtime_error("Invalid argument");
	}
	
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	// 채널명 있는지 확인
	Channel *channel = server->getChannelByChannelName(token[1].substr(1));
	if (channel == NULL)
		throw std::runtime_error("Channel doesn't exist");
	
	// 입력 유저가 channel에 있는지, operator인지 확인
	if (channel->checkClientExistByClientFd(client->getFd()) == 0)
		throw std::runtime_error("User is not in channel");
	if (channel->isOperator(client->getFd()) == 0)
		throw std::runtime_error("User is not operator");
	
	// kickClients (token[2] : nickname list)
	std::vector<std::string> kickClients = split(token[2], ',');
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
		server->noticeToClient(kickClient->getFd(), "You are kicked from " + token[1]);
		kickClient->removeChannel(token[2].substr(1));

		std::cout << "kick " << kickClient->getFd() << " from " << token[1] << std::endl;
		
		std::stringstream ss;
		ss << "Kick " << kickClient->getFd() << " from " << token[1] << "\n";
		std::string kickMessage = ss.str();
		channel->broadcast(kickMessage);

		std::string message = "There is no reason\n";
		if (token.size() == 4)
			message = "Client" + client->getNickName() + " kicked because " + token[3].substr(1) + '\n';
		channel->broadcast(message);
	}
}

void	Command::invite(Server *server, Client *client, std::vector<std::string> token)
{
	if (token.size() != 3 || token[2][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	std::string nickName = token[1];
	std::string channelName = token[2].substr(1);
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
	if (token.size() < 2 || token[1][0] != '#')
		throw std::runtime_error("Invalid argument");
	
	// 채널명 있는지 확인
	Channel *channel = server->getChannelByChannelName(token[1].substr(1));
	if (channel == NULL)
		throw std::runtime_error("Channel doesn't exist");
	
	// 입력 유저가 channel에 있는지, operator인지, channel mode에 't' 있는지 확인
	if (channel->checkClientExistByClientFd(client->getFd()) == 0)
		throw std::runtime_error("User is not in channel");

	if (token.size() == 2)
	{
		std::string message = channel->getTopic() + "\n";
		client->addWriteBuff(message);
	}
	else
	{
		if (client->getAuthorized() < 3)
			throw std::runtime_error("Not authorized");
		if (token[2][0] != ':')
			throw std::runtime_error("Invalid argument");
		if (channel->getMode().find('t') != std::string::npos)
		{
			if (channel->isOperator(client->getFd()) == 0)
				throw std::runtime_error("User is not operator");
		}

		std::string topic = "";
		token[2] = token[2].substr(1);

		size_t i = 2;
		while (i < token.size())
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
	if (token.size() != 3 && token.size() != 4)
		throw std::runtime_error("Invalid argument");
	if (token[1][0] != '#')
		throw std::runtime_error("Invalid argument");
	if (client->getAuthorized() < 3)
		throw std::runtime_error("Not authorized");
	
	// 채널명 있는지 확인
	Channel *channel = server->getChannelByChannelName(token[1].substr(1));
	if (channel == NULL)
		throw std::runtime_error("Channel doesn't exist");
	
	// 입력 유저가 channel에 있는지, operator인지 확인
	if (channel->isOperator(client->getFd()) == 0)
		throw std::runtime_error("User is not operator");
	std::vector<std::string> args;
	int size = 0;
	if (token.size() == 4)
	{
		args = split(token[3], ',');
		size = args.size();
	}

	int i = 0;
	int j = 0;
	int flag = -1;
	int visited[7] = {0, 0, 0, 0, 0, 0, 0};
	while (token[2][i])
	{
		if (token[2][i] != '+' && token[2][i] != '-' && token[2][i] != 'i' && token[2][i] != 't' && token[2][i] != 'k' && token[2][i] != 'o' && token[2][i] != 'l')
			throw std::runtime_error("Invalid argument");

		if (token[2][i] == 'i' && !visited[0])
		{
			visited[0] = 1;
			if (flag)
				channel->setMode('i');
			else
				channel->removeMode('i');
		}
		else if (token[2][i] == 't' && !visited[1])
		{
			visited[1] = 1;
			if (flag)
				channel->setMode('t');
			else
				channel->removeMode('t');
		}
		else if (token[2][i] == 'k' && !visited[2])
		{
			visited[2] = 1;
			if (flag)
			{
				if (j >= size || args[j].empty())
					throw std::runtime_error("Invalid argument");
				channel->setKey(args[j++]);
			}
			else
				channel->setKey("");
		}
		else if (token[2][i] == 'o' && !visited[3])
		{
			visited[3] = 1;
			if (j >= size || args[j].empty())
				throw std::runtime_error("Invalid argument");
			Client *opClient = channel->getClientByNickname(args[j++]);
			if (opClient == NULL)
				throw std::runtime_error("User is not in channel");
			if (flag)
				channel->setOperator(opClient);
			else
				channel->removeOperator(opClient);
		}
		else if (token[2][i] == 'l' && !visited[4])
		{
			visited[4] = 1;
			if (flag)
			{
				if (j >= size || args[j].empty())
					throw std::runtime_error("Invalid argument");
				channel->setMode('l');
				int limit = atoi(args[j++].c_str());
				if (limit < 0)
					return;
				channel->setLimit(limit);
			}
			else
			{
				channel->removeMode('l');
				channel->setLimit(-1);
			}
		}
		else if (token[2][i] == '+' && !visited[5])
		{
			flag = 1;
			visited[5] = 1;
		}
		else if (token[2][i] == '-' && !visited[6])
		{
			flag = 0;
			visited[6] = 1;
		}
		else
			throw std::runtime_error("Invalid argument");
		i++;
	}
}

std::vector<std::string>	Command::split(std::string str, char delimiter)
{
	std::vector<std::string> ret;
	size_t pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		if (pos != 0)
			ret.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}
	if (!str.empty())
		ret.push_back(str);
	return ret;
}

std::vector<std::string>	Command::key_split(std::string str, char delimiter)
{
	std::vector<std::string> ret;
	size_t pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		ret.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}
	if (!str.empty())
		ret.push_back(str);
	return ret;
}
