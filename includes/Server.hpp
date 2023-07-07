#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Parsing.hpp"
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>

class Server
{
	private:
		int			_socket;
		int			_port;
		std::string	_password;
	
	public:
		Server(char *port, char *password);
};

#endif
