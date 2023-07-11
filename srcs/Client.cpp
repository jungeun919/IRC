#include "Client.hpp"

Client::Client(int fd)
{
	_fd = fd;
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
