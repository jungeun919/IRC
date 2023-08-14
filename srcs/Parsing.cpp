#include "Parsing.hpp"
#include "Command.hpp"

std::vector<std::string> Parsing::parsing(std::string command)
{
	std::vector<std::string> token = Command::split(command, ' ');
	return token;
}

int Parsing::checkCommand(std::string cmd)
{
	std::string	cmd_list[9] = {
		"PASS",
		"NICK",
		"USER",
		"JOIN",
		"PRIVMSG",
		"KICK",
		"INVITE",
		"TOPIC",
		"MODE",
	};

	for (int i = 0; i < 9; i++)
	{
		if (cmd == cmd_list[i])
			return 1;
	}

	return 0;
}