#include "Client.hpp"

Client::Client(int fd)
{
	_fd = fd;
	_authorized = 0;
}

void	Client::addReadBuff(std::string message)
{
	_readBuff += message;
}

void	Client::addWriteBuff(std::string message)
{
	_writeBuff += message;
}

std::string&	Client::getReadBuff(void)
{
	return _readBuff;
}

std::string&	Client::getWriteBuff(void)
{
	return _writeBuff;
}

int Client::getAuthorized(void)
{
	return _authorized;
}

void Client::setAuthorized(int authorized)
{
	_authorized = authorized;
}

std::string& Client::getNickName(void)
{
	return _nickName;
}

void Client::setNickName(std::string nickName)
{
	_nickName = nickName;
}