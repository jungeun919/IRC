#pragma once

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

class Client
{
	private:
		int			_fd;
		std::string	_readBuff;
		std::string	_writeBuff;

		int 		_authorized;
		std::string _nickName;
	
	public:
		Client(int fd);

		void			addReadBuff(std::string message);
		void			addWriteBuff(std::string message);
		
		std::string&	getReadBuff(void);
		std::string&	getWriteBuff(void);

		int				getAuthorized(void);
		void			setAuthorized(int authorized);

		std::string&	getNickName(void);
		void			setNickName(std::string nickName);
};

#endif
