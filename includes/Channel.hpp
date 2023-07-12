#pragma once

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"

# include <iostream>
# include <map>

class Client;

class Channel
{
	private:
		std::string				_name;
		std::map<int, Client*>	_clientList;
		std::map<int, Client*>	_operatorList;
		std::string				_key;
	
	public:
		Channel(std::string name);

		std::string&			getName(void);
		std::map<int, Client*>&	getClientList(void);
		
		void					addClient(Client* client);
		void					removeClient(int clientFd);
		void					broadcast(std::string message, Client* client);

		std::string 			getKey(void);
		void					setKey(std::string key);
};

#endif
