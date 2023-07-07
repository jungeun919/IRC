#include "Parsing.hpp"

std::vector<std::string> Parsing::parsing(char *buf)
{
	std::vector<std::string> token;
	char *ptr = strtok(buf, " ");
	while (ptr != NULL)
	{
		token.push_back(static_cast<std::string>(ptr));
		ptr = strtok(NULL, " ");
	}
	return token;
}