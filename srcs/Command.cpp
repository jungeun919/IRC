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
	else if (token[1] == "USER")
		user(0, token[1]);
	else if (token[1] == "JOIN")
		join(0, token[1]);
	else if (token[1] == "PRIVMSG")
		privmsg(0, token[2], token[3]);
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
	if (client->getAuthorized() == 0)
		throw std::runtime_error("Not authorized");
	
	if (server->checkNickName(nickName))
		throw std::runtime_error("NickName already exist");
	
	client->setNickName(nickName);
}

void	Command::user(int fd, std::string userName)
{
	static_cast<void>(fd);
	static_cast<void>(userName);
	std::cout << "USER" << std::endl;
}

void	Command::join(int fd, std::string channelName)
{
	static_cast<void>(fd);
	static_cast<void>(channelName);
	std::cout << "JOIN" << std::endl;
}

void	Command::privmsg(int fd, std::string channelName, std::string message)
{
	static_cast<void>(fd);
	static_cast<void>(channelName);
	static_cast<void>(message);
	std::cout << "PRIVMSG" << std::endl;
}