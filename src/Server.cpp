#include "Server.hpp"

void Server::exitWithError(std::string errorMessage)
{
	perror(errorMessage.c_str());
	exit(1);
}

void Server::log(const std::string &message)
{
	std::cout << message << std::endl;
}

Server::Server(std::string port, std::string password)
{
	_socket = -1;
	_bin = -1;
	_lis = -1;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
	{
		exitWithError("Error establishing the server socket");
	}
	_port = atoi(port.c_str());
	_password = password;
	if (_port < 1024 || _port > 65535)
	{
		std::cout << "bad value of port" << std::endl;
		return ;
	}
	if (_socket < 0)
	{
		std::cerr << "sock error" << std::endl;
		return ;
	}
	memset((char *)&_sin, 0, sizeof(_sin)); // Use memset instead of bzero
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(_port);
	_sin.sin_addr.s_addr = INADDR_ANY;
	_bin = bind(_socket, (struct sockaddr *)&_sin, sizeof(_sin));
	if (_bin < 0)
	{
		exitWithError("Error binding socket to local address");
	}
	_lis = listen(_socket, 5);
	if (_lis)
	{
		std::cerr << "listen error" << std::endl;
		return ;
	}
}

Server::~Server()
{
	close(_socket);
}

void Server::serving() {
    int max_sd, new_socket;
    fd_set rfds, tmp_rfds;
    struct sockaddr_in client;
    socklen_t addr_len = sizeof(client);
    std::map<int, Client*> _client; // Corrected map type

    FD_ZERO(&rfds);
    FD_SET(_socket, &rfds);
    max_sd = _socket;

    std::cout << "Server is waiting for connections on port " << _port << std::endl;

    while (true)
    {
        tmp_rfds = rfds;

        if (select(max_sd + 1, &tmp_rfds, NULL, NULL, NULL) == -1) {
            std::cerr << "select error" << std::endl;
            break;
        }

        if (FD_ISSET(_socket, &tmp_rfds)) {
            if ((new_socket = accept(_socket, (struct sockaddr *)&client, &addr_len)) == -1) {
                std::cerr << "accept error" << std::endl;
                break;
            } else {
                std::cout << "New connection from " << inet_ntoa(client.sin_addr) << " on socket " << new_socket << std::endl;
                _client[new_socket] = new Client(new_socket, client);
				std::cout << "size MAP : "<< _client.size() << std::endl;
                FD_SET(new_socket, &rfds);
                if (new_socket > max_sd)
                    max_sd = new_socket;
            }
        }
        for (std::map<int, Client*>::iterator it = _client.begin(); it != _client.end();)
        {
            int client_socket = it->first;
            if (FD_ISSET(client_socket, &tmp_rfds)) {
                char buffer[1024];
                int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0)
                {
                    std::cout << "Client on socket " << client_socket << " " << it->second->getUsername() << " disconnected." << std::endl;
                    close(client_socket);
                    FD_CLR(client_socket, &rfds);
                    delete it->second;
                    _client.erase(it);
                }
                else
                {
                    buffer[bytes_received] = '\0';
                    std::cout << std::endl;
					std::string msg(buffer);
					for (std::map<int, Client*>::iterator it = _client.begin();it != _client.end();it++)
					{
						send(it->second->_socket_fd, msg.c_str(), msg.size(), 0);
						std::cout << "BUFFER = " << buffer << it->second->_socket_fd <<  std::endl;
					}
					std::cout << "<<<<" << _client.size() << std::endl;
                    parse(it->second, buffer);
                    std::cout << "Received from client " << client_socket << ": [" << buffer << "]" << std::endl;
                    ++it;
                }
            } else
            {
                ++it;
            }
        }
    }
    close(_socket);
}

int	cmdparser(const std::string &str)
{
	if (str.compare(0, 4, "PASS") == 0)
		return (4);
	if (str.compare(0, 4, "NICK") == 0)
		return (4);
	if (str.compare(0, 4, "USER") == 0)
		return (4);
	return (0);
}

void prvmsg(std::string dest)
{
    std::cout << "<<<<<<<<<<" << dest << std::endl; 
    // for (std::vector<Client>::iterator it = it.begin(); it != it.end();)
    // {
    //     if (client->getNickname == dest)
    //         send(client->socket, msg.c_str(), msg.size(), 0);
    // }
}

void Server::exec_cmd(std::string const &command, std::string const &value,
	Client *client)
{
	if (command == "PASS")
	{
		client->setPassword(value);
		client->checkplus();
	}
	else if (command == "NICK")
	{
		client->setNickname(value);
		client->checkplus();
	}
	else if (command == "USER")
	{
		client->setUsername(value);
		client->checkplus();
	}
}

void Server::set_id(std::string str, Client *client)
{
	size_t	i;
	size_t	cmdStart;
	size_t	cmdEnd;
	size_t	valueStart;
	size_t	valueEnd;

	i = 0;
	while (i < str.size())
	{
		cmdStart = str.find_first_of("PASS, NICK, USER", i);
		if (cmdStart == std::string::npos)
			break ;
		cmdEnd = str.find_first_of(" \r\n", cmdStart);
		if (cmdEnd == std::string::npos)
			break ;
		std::string command = str.substr(cmdStart, cmdEnd - cmdStart);
		std::cout << "Command: " << command << std::endl;
		valueStart = str.find_first_not_of(" \r\n", cmdEnd);
		if (valueStart == std::string::npos)
			break ;
		valueEnd = str.find_first_of(" \r\n", valueStart);
		if (valueEnd == std::string::npos)
			break ;
		std::string value = str.substr(valueStart, valueEnd - valueStart);
		std::cout << "Value: " << value << std::endl;
		if (cmdparser(command) == 4)
		{
			exec_cmd(command, value, client);
		}
		i = valueEnd + 2;
	}
}

bool Server::cmd_pars(Client *client, std::string buffer)
{
	size_t	i;
	size_t	cmdStart;
	size_t	cmdEnd;
	size_t	valueStart;
	size_t	valueEnd;
	size_t	pass;
	size_t	passend;

	i = 0;
	cmdStart = buffer.find_first_of("JOIN", i);
	std::string passvalue;
	if (cmdStart == std::string::npos)
		return (false);
	cmdEnd = buffer.find_first_of(" ", cmdStart);
	if (cmdEnd == std::string::npos)
		return (false);
	std::string command = buffer.substr(cmdStart, cmdEnd - cmdStart);
	std::cout << "Command: " << command << std::endl;
	valueStart = buffer.find_first_not_of(" #", cmdEnd);
	if (valueStart == std::string::npos)
		return (false);
	valueEnd = buffer.find_first_of(" \r\n", valueStart);
	if (valueEnd == std::string::npos)
		return (false);
	std::string value = buffer.substr(valueStart, valueEnd - valueStart);
	std::cout << "Value: " << value << std::endl;
	if (command != "JOIN")
		return (false);
	pass = buffer.find_first_not_of(" ", valueEnd);
	passend = buffer.find_first_of(" \r\n", pass);
	if (passend != std::string::npos && pass != std::string::npos)
	{
		std::string passvalue = buffer.substr(pass, passend - valueStart);
		std::cout << "pass: " << passvalue << std::endl;
	}
	else
	{
		std::string badchan = ":irc.example.com 432 : Vous n'êtes pas sur ce canal.\r\n";
		send(client->_socket_fd, badchan.c_str(), badchan.size(), 0);
		std::cout << "Erreur" << std::endl;
	}
	joinChannel(client, value, passvalue);
	return (true);
}

Client *Server::parse(Client *client, std::string buffer)
{
	size_t	bufferSize;

	bufferSize = buffer.size();
	if (buffer[bufferSize - 1] == '\n') //&& buffer[bufferSize - 2] == '\r')
	{
		if (!client->getRegister())
		{
			set_id(buffer, client);
			if (client->getCheck() == 3)
			{
				// Construct the welcome message for the Irssi client
				std::string welcome = ":irc.example.com 001 "
					+ client->getNickname()
					+ " :Welcome to the IRC Network BRO\r\n";
				send(client->_socket_fd, welcome.c_str(), welcome.size(), 0);
				client->setRegister(true);
				// buffer.clear();
				return (client);
			}
		}
		else if (client->getRegister())
		{
			// std::cout << "BUFFER = " << buffer << std::endl;
			std::cout << _client.size() << std::endl;
			for (std::map<int, Client*>::iterator it = _client.begin();it != _client.end();it++)
			{
				send(it->second->_socket_fd, buffer.c_str(), buffer.size(), 0);
				std::cout << "BUFFER = " << buffer << it->second->_socket_fd <<  std::endl;
			}
			if (cmd_pars(client, buffer))
			{
			}
			else
			{
				// buffer.clear();
			}
			return (client);
		}
	}
	std::cout << "ups" << std::endl;
	return (NULL);
}

std::string Server::getPassword()
{
	return (_password);
}
void Server::printserv()
{
}
Client *Server::getClient(int socket)
{
	if (_client.find(socket) != _client.end())
	{
		return (_client[socket]);
	}
	return (NULL);
}
Channel *Server::getChannel(std::string channelname)
{
	if (_channel.find(channelname) != _channel.end())
	{
		return (_channel[channelname]);
	}
	return (NULL);
}

void Server::joinChannel(Client *client, std::string channelname,std::string password)
{
	if (client)
	{
		if (_channel.find(channelname) == _channel.end())
		{
            std::cout << "created channel" << std::endl;
			_channel[channelname] = new Channel(client, channelname, password);
		}
		else
		{
			if (!password.empty() || (_channel[channelname]->getNeed() && (strcmp(password.c_str(), _channel[channelname]->getPassword().c_str()) != 0)))
			{
                std::cout << "NOOOOOO" << std::endl;
                std::string error_passwd = ":ERR_BADCHANNELKEY" +_channel[channelname]->getName() + " :Password incorrect\r\n";
                send(client->_socket_fd, error_passwd.c_str(), error_passwd.size(), 0);
				return ;
			}
				std::string goodjoin = ":" + client->getNickname() + "!"
	            + client->getUsername() + "@localhost JOIN :#" + _channel[channelname]->getName() + "\r\n";
	            send(client->_socket_fd, goodjoin.c_str(), goodjoin.size(), 0);
            _channel[channelname]->addClients(client);
            std::cout << "---------------------" << _channel[channelname]->getNeed() << strcmp(password.c_str(), _channel[channelname]->getPassword().c_str()) << std::endl;
		}
	}
}