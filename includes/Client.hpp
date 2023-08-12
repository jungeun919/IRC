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
	std::string	_readBuff;
	std::string	_writeBuff;

	std::map<std::string, Channel*> _channelList;

public:
	Client(int fd);
	~Client(void);

	int				getFd(void);
	int				getAuthorized(void);
	std::string&	getUserName(void);
	std::string&	getRealName(void);
	std::string&	getNickName(void);
	std::string&	getReadBuff(void);
	std::string& 	getWriteBuff(void);

	std::map<std::string, Channel*>	getChannelList(void);

	void			setAuthorized(int authorized);
	void			setUserName(std::string userName);
	void			setRealName(std::string realName);
	void			setNickName(std::string nickName);

	void			addReadBuff(std::string message);
	void			addWriteBuff(std::string message);
	void			addChannel(std::string channelName, Channel* channel);
	void			removeChannel(std::string channelName);
};

#endif
