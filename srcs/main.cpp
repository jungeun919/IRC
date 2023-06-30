#include "../includes/irc.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Invalid argument" << std::endl;
		std::cout << "Follow this format: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	(void)argv;
	
	return 0;
}
