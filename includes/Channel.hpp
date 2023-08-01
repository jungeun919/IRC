#pragma once

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"

# include <iostream>
# include <map>
# include <sys/socket.h>

class Client;

class Channel
{
	private:
		std::string				_name;
		std::string				_key;
		std::string				_topic;
		std::string				_mode;
		int						_limit;
	
		std::map<int, Client*>	_operatorList;
		std::map<int, Client*>	_clientList;

	public:
		Channel(std::string name);

		std::string&			getName(void);
		std::string&			getTopic(void);

		void					setTopic(std::string topic);
		void					clearTopic(void);
		
		void					addClient(Client* client);
		void					removeClient(int clientFd);
		void					broadcast(std::string message);

		std::string 			getKey(void);
		void					setKey(std::string key);

		std::string 			getMode(void);
		void					setMode(char mode);
		void					removeMode(char mode);

		int 					getLimit(void);
		void					setLimit(int limit);

		std::map<int, Client*>	getOperatorList(void);
		std::map<int, Client*>	getClientList(void);
		void					setOperator(Client* client);
		void					removeOperator(Client* client);

		int						checkClientExistByClientFd(int clientFd);
		int						isOperator(int clientFd);

		Client*					getClientByNickname(std::string nickName);
};

#endif
