#pragma once

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"

# include <iostream>
# include <map>

class Channel
{
	private:
		std::string				_name;
		std::map<int, Client*>	_clientList;
		std::map<int, Client*>	_operatorList;
	
	public:
		Channel(std::string name);

		std::string&			getName(void);
		std::map<int, Client*>&	getClientList(void);
		
		void					addClient(int clientFd, Client* client);
		void					removeClient(int clientFd);
		void					broadcast(std::string message, Client* client);
};

#endif
