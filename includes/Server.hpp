#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_EVENTS 10
# define BUFFER_SIZE 1024

# include "Parsing.hpp"
# include "Client.hpp"
# include "Command.hpp"
# include "Channel.hpp"

# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>

# include <vector>
# include <map>
# include <fcntl.h>

class Server
{
	private:
		int							_socket;
		int							_port;
		std::string					_password;

		int							_kqueue;
		std::vector<struct kevent>	_changeList;
		struct kevent				_eventList[MAX_EVENTS];

		std::map<int, Client*>		_clientList;
	public:
		std::map<std::string, Channel*>	_channelList;
		
		Server(char *port, char *password);

		void	initKqueue(void);
		void	addEvents(int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void	run(void);
		void	handleEvent(struct kevent &event);
		void	disconnectClient(uintptr_t clientFd);

		std::string 	getPassword(void);
		int				checkUserName(std::string userName);
		int				checkRealName(std::string channelName);
		int 			checkNickName(std::string nickName);
		int 			checkChannelName(std::string channelName);

		std::map<int, Client*>		getClientList(void);
		void			addChannel(std::string channelName);
};

#endif
