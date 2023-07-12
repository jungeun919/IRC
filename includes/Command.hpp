#pragma once

#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "Server.hpp"
# include "Client.hpp"
# include <iostream>
# include <vector>

class Server;

class Command
{
public:
	static void	runCommand(std::vector<std::string> token, Server *server, Client *client);
	static void	pass(Server *server, Client *client, std::string password);
	static void	nick(Server *server, Client *client, std::string nickName);
	static void	user(Server *server, Client *client, std::string userName, std::string realName);
	static void	join(int fd, std::string channelName);
	static void	privmsg(int fd, std::string channelName, std::string message);
};

#endif