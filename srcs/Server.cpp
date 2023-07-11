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
	
	/*
	// 4. accept
	struct sockaddr_in client_addr;
	unsigned int size = sizeof(client_addr);
	int client_socket;
	if ((client_socket = accept(_socket, (struct sockaddr*)&client_addr, &size)) == -1)
		throw std::runtime_error("Failed to accept");

	// 5. recv
	char buf[1024];
	int len;
	if ((len = recv(client_socket, buf, 1024, 0)) == -1)
		throw std::runtime_error("Failed to recv");
	buf[len] = '\0';
	std::cout << buf << std::endl;

	// 5.5. parsing
	std::vector<std::string> token = Parsing::parsing(buf);
	for (std::vector<std::string>::iterator it = token.begin(); it != token.end(); ++it)
		std::cout << *it << std::endl;

	// 6. send
	if (send(client_socket, "Hello, world!\n", 14, 0) == -1)
		throw std::runtime_error("Failed to send");
	
	// 7. close
	close(client_socket);
	close(_socket);
	*/
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
	std::cout << event.data << std::endl;
}

void	Server::run(void)
{
	initKqueue();

	while (true)
	{
		int n;
		n = kevent(_kqueue, _changeList.data(), _changeList.size(), _eventList, MAX_EVENTS, NULL);
		if (n == -1)
			throw std::runtime_error("Failed to fetch event");
		_changeList.clear();

		for (int i = 0; i < n; ++i)
			handleEvent(_eventList[i]);
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
			if (bytes <= 0)
				disconnectClient(event.ident);
			else
			{
				std::map<int, Client*>::iterator it = _clientList.find(event.ident);
				if (it == _clientList.end())
					return ;
				
				buff[bytes] = '\0';
				it->second->addReadBuff(static_cast<std::string>(buff));
				// parsing string & command 동작 코드 추가
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
		if (bytes <= 0)
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
