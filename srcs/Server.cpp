#include "Server.hpp"

Server::Server(char *port, char *password)
{
	// 1. create socket
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Failed to create server socket");
	
	int option = true;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		throw std::runtime_error("Failed to set socket option"); 

	// 2. bind
	_port = std::atoi(port);
	if (_port < 0 || _port > 65536)
		throw std::range_error("Invalid port range");
	_password = static_cast<std::string>(password);
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(_port);

	if (bind(_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("Failed to bind");
	
	// 3. listen
	if (listen(_socket, 100) == -1) // max_client
		throw std::runtime_error("Failed to listen");
}

void	Server::initKqueue(void)
{
	if ((_kqueue = kqueue()) == -1)
		throw std::runtime_error("Failed to create kqueue");
}

void	Server::addEvents(int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent event;

    EV_SET(&event, socket, filter, flags, fflags, data, udata);
    _changeList.push_back(event);
}

void	Server::run(void)
{
	initKqueue();

	addEvents(_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    std::cout << "echo server started" << std::endl;
	while (true)
	{
		int n;
		n = kevent(_kqueue, _changeList.data(), _changeList.size(), _eventList, MAX_EVENTS, NULL);
		if (n == -1)
			throw std::runtime_error("Failed to fetch event");
		_changeList.clear();
		
		for (int i = 0; i < n; ++i)
		{
			try
			{
				handleEvent(_eventList[i]);
			}
			catch (std::exception &e)
			{
				std::cout << e.what() << std::endl;
				std::string msg = e.what();
				msg = "[Server] " + msg + "\n";
				if (send(_eventList[i].ident, msg.c_str(), msg.length(), 0) == -1)
					throw std::runtime_error("Failed to send");
			}
		}
	}
}

void	Server::handleEvent(struct kevent &event)
{
	if (event.flags & EV_ERROR)
	{
		if (event.ident == (uintptr_t)_socket)
			throw std::runtime_error("Server socket error");
		else
		{
			std::cout << "Client socket error";
			// disconnect client 추가 (ok)
			disconnectClient(event.ident);
		}
	}
	else if (event.filter == EVFILT_READ)
	{
		if (event.ident == (uintptr_t)_socket)
		{
			// accept new client
			struct sockaddr_in client_addr;
			unsigned int size = sizeof(client_addr);
			int client_socket;
			if ((client_socket = accept(_socket, (struct sockaddr*)&client_addr, &size)) == -1)
				throw std::runtime_error("Failed to accept");
			fcntl(client_socket, F_SETFL, O_NONBLOCK);
			
			// add event for client socket
			addEvents(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			addEvents(client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			// client 동작 추가 (ok)
			Client* client = new Client(client_socket);
			_clientList.insert(std::make_pair(client_socket, client));
		}
		else
		{
			// read data from client 코드 추가 (ok)
			char buff[BUFFER_SIZE];
			memset(buff, 0, BUFFER_SIZE);

			int bytes = recv(event.ident, buff, BUFFER_SIZE, 0);
			if (bytes < 0)
				disconnectClient(event.ident);
			else
			{
				std::map<int, Client*>::iterator it = _clientList.find(event.ident);
				if (it == _clientList.end())
					return ;
				
				buff[bytes] = '\0';
				it->second->addReadBuff(static_cast<std::string>(buff));
				std::cout << it->first << " : " << buff;
				// parsing string & command 동작 코드 추가
				std::vector<std::string> token = Parsing::parsing(buff);
			if (token[0] == "1")
				Command::runCommand(token, this, it->second);
			else
				std::cout << "not command" << std::endl;
			}
		}

	}
	else if (event.filter == EVFILT_WRITE)
	{
		// send data to client 코드 추가 (ok)
		std::map<int, Client*>::iterator it = _clientList.find(event.ident);
		if (it == _clientList.end())
			return ;
		
		std::string& message = it->second->getWriteBuff();
		int bytes = send(event.ident, message.c_str(), message.length(), 0);
		if (bytes < 0)
			disconnectClient(event.ident);
	}
}

void	Server::disconnectClient(uintptr_t clientFd)
{
	std::map<int, Client*>::iterator it = _clientList.find(clientFd);
	if (it == _clientList.end())
		return ;
	_clientList.erase(clientFd);
	delete it->second;
}

std::string Server::getPassword(void)
{
	return _password;
}

int Server::checkUserName(std::string userName)
{
	std::map<int, Client*>::iterator it = _clientList.begin();
	while (it != _clientList.end())
	{
		if (it->second->getUserName() == userName)
			return 1;
		it++;
	}
	return 0;
}

int Server::checkRealName(std::string realName)
{
	std::map<int, Client*>::iterator it = _clientList.begin();
	while (it != _clientList.end())
	{
		if (it->second->getRealName() == realName)
			return 1;
		it++;
	}
	return 0;
}

int Server::checkNickName(std::string nickName)
{
	std::map<int, Client*>::iterator it = _clientList.begin();
	while (it != _clientList.end())
	{
		if (it->second->getNickName() == nickName)
			return 1;
		it++;
	}
	return 0;
}

int Server::checkChannelName(std::string channelName)
{
	std::map<std::string, Channel*>::iterator it = _channelList.begin();
	while (it != _channelList.end())
	{
		if (it->first == channelName)
			return 1;
		it++;
	}
	return 0;
}

std::map<int, Client*> Server::getClientList(void)
{
	return _clientList;
}

void Server::addChannel(std::string channelName)
{
	_channelList.insert(std::make_pair(channelName, new Channel(channelName)));
}

int Server::getFdByNickName(std::string nickName)
{
	std::map<int, Client*>::iterator it = _clientList.begin();
	while (it != _clientList.end())
	{
		if (it->second->getNickName() == nickName)
			return it->first;
		it++;
	}
	return -1;
}