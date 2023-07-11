#include "Server.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Invalid argument" << std::endl;
		std::cout << "Follow this format: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	
	try
	{
		Server	server(argv[1], argv[2]);
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}
