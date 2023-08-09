#pragma once

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Channel.hpp"

# include <map>
# include <iostream>
# include <unistd.h>

class Channel;

class Client
{
	private:
		int			_fd;
		int 		_authorized;
		std::string _userName;
		std::string _realName;
		std::string _nickName;
	
		std::map<std::string, Channel*> _channelList;
	
	public:
		Client(int fd);
		~Client(void);

		int				getFd(void);

		int				getAuthorized(void);
		void			setAuthorized(int authorized);

		std::string&	getUserName(void);
		void			setUserName(std::string userName);

		std::string&	getRealName(void);
		void			setRealName(std::string realName);

		std::string&	getNickName(void);
		void			setNickName(std::string nickName);

		std::map<std::string, Channel*>	getChannelList(void);
		void			addChannel(std::string channelName, Channel* channel);
		void			removeChannel(std::string channelName);
};

#endif
