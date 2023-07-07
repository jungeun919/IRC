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
}
