#include "Parsing.hpp"

std::vector<std::string> Parsing::parsing(char *buf)
{
	std::vector<std::string> token;
	char *ptr = strtok(buf, " ");
	if (checkCommand(ptr))
		token.push_back("1");
	else
		token.push_back("0");
	
	while (ptr != NULL)
	{
		token.push_back(static_cast<std::string>(ptr));
		ptr = strtok(NULL, " ");
	}

	if (!token.empty())
	{
  		std::string& lastElement = token.back();
  		if (!lastElement.empty())
    		lastElement = lastElement.substr(0, lastElement.size() - 1);
	}
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