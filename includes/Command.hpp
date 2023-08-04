#pragma once

#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "Server.hpp"
# include "Client.hpp"

# include <iostream>
# include <vector>
# include <sys/socket.h>

class Server;

class Command
{
public:
	static void	runCommand(std::vector<std::string> token, Server *server, Client *client);
	static void	pass(Server *server, Client *client, std::string password);
	static void	nick(Server *server, Client *client, std::string nickName);
	static void	user(Server *server, Client *client, std::string userName, std::string realName);
	static void	join(Server *server, Client *client, std::vector<std::string> token);
	static void	privmsg(Server *server, Client *client, std::vector<std::string> token);
	static void kick(Server *server, Client *client, std::vector<std::string> token);
	static void invite(Server *server, Client *client, std::string nickName, std::string channelName);
	static void topic(Server *server, Client *client, std::vector<std::string> token);
	static void mode(Server *server, Client *client, std::vector<std::string> token);

	static std::vector<std::string>	split(std::string str, std::string delimiter);
};

#endif
