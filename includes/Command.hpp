#pragma once

#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "Server.hpp"
# include "Client.hpp"

# include <iostream>
# include <sstream>
# include <vector>
# include <sys/socket.h>

class Server;

class Command
{
public:
	static void	runCommand(std::vector<std::string> token, Server *server, Client *client);
	static void	pass(Server *server, Client *client, std::vector<std::string> token);
	static void	nick(Server *server, Client *client, std::vector<std::string> token);
	static void	user(Server *server, Client *client, std::vector<std::string> token);
	static void	join(Server *server, Client *client, std::vector<std::string> token);
	static void	privmsg(Server *server, Client *client, std::vector<std::string> token);
	static void kick(Server *server, Client *client, std::vector<std::string> token);
	static void invite(Server *server, Client *client, std::vector<std::string> token);
	static void topic(Server *server, Client *client, std::vector<std::string> token);
	static void mode(Server *server, Client *client, std::vector<std::string> token);

	static std::vector<std::string>	split(std::string str, char delimiter);
	static std::vector<std::string>	key_split(std::string str, char delimiter);
};

#endif
