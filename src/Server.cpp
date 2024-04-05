/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:21:41 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/05 18:43:22 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h> 

void Server::exitWithError(std::string errorMessage)
{
	perror(errorMessage.c_str());
	// exit(1);
}


std::string Server::getPassword()
{
	return (_password);
}
void Server::printserv()
{
}
Client *Server::getClient(std::string name)
{
	std::map<int, Client *>::iterator it = _client.begin();
	while(it != _client.end())
	{
		if (it->second->getUsername() == name)
		{
			return (it->second);
		}
		it++;
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
	int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return;
    }
    if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        return;
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
		return;
		// exitWithError("Error binding socket to local address");
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
//    std::cout << "Wesh" << std::endl;
	// delete _client["new"];
    for (std::map<int , Client*>::iterator it = _client.begin(); it != _client.end(); it++)
    {
        delete it->second;
    }
    _client.clear();
    for (std::map<std::string , Channel*>::iterator it = _channel.begin(); it != _channel.end(); it++)
    {
        delete it->second;
    }
	_channel.clear();
}

void Server::serving() {
    int max_sd, new_socket;
    fd_set rfds, tmp_rfds;
    struct sockaddr_in client;
    socklen_t addr_len = sizeof(client);

    FD_ZERO(&rfds);
    FD_SET(_socket, &rfds);
    max_sd = _socket;

    std::cout << "Server is waiting for connections on port " << _port << std::endl;


    while (server_off == false)
    {
    	tmp_rfds = rfds;
		if (select(max_sd + 1, &tmp_rfds, NULL, NULL, NULL) == -1) {
			return;
		}
        if (FD_ISSET(_socket, &tmp_rfds)) {
            if ((new_socket = accept(_socket, (struct sockaddr *)&client, &addr_len)) == -1) {
                std::cerr << "accept error" << std::endl;
                return;
            } else {
                std::cout << "New connection from " << inet_ntoa(client.sin_addr) << " on socket " << new_socket << std::endl;
				_client[new_socket] = new Client(new_socket, client);
                FD_SET(new_socket, &rfds);
                if (new_socket > max_sd)
                    max_sd = new_socket;
            }
        }
        for (std::map<int , Client*>::iterator it = _client.begin(); it != _client.end();)
        {
            int client_socket = it->first;
            if (FD_ISSET(client_socket, &tmp_rfds)) {
                char buffer[1024];
                int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0)
                {
                    std::cout << "Client on socket " << client_socket << " " << it->second->getUsername() << " disconnected." << std::endl;
                    FD_CLR(client_socket, &rfds);
					delete it->second;
					_client.erase(it);
					it = _client.begin();
					close(client_socket);
                }
                else
                {
                    buffer[bytes_received] = '\0';
                    std::cout << std::endl;
                    parse(it->second, buffer);
                    std::cout << "Received from client " << client_socket << ": [" << buffer << "]" << std::endl;
                    ++it;
                }
            }
			else
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

void Server::exec_cmd(std::string const &command, std::string const &value,
	Client *client)
{
	if (command == "PASS")
	{
		if (value != this->getPassword())
		{
			std::cout << "BAD PASSWORD" << std::endl;
		}
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

int	Server::which_command(std::string cmd)
{
	if (cmd == "JOIN")
		return (0);
	if (cmd == "KICK")
		return (1);
	if (cmd == "INVITE")
		return (2);
	if (cmd == "TOPIC")
		return (3);
	if (cmd == "PRIVMSG")
		return (4);
	if (cmd == "MODE")
		return (5);
	return (-1);
}

void	Server::cmd_pars(Client *client, std::string buffer)
{
	try {
		size_t	cmdEnd = buffer.find_first_of(" \t\r\n", 0);
		if (cmdEnd == std::string::npos)
			throw std::string("Command does not exist");
		std::string	cmd = buffer.substr(0, cmdEnd);

		switch (which_command(cmd)) {
			case 0:
				join_pars(client, buffer);
				break;
			case 1:
				kick_pars(client, buffer);
				break;
			case 2:
				invite_pars(client, buffer);
				break;
			case 3:
				topic_pars(client, buffer);
				break;
			case 4:
				privmsg_pars(client, buffer);
				break;
			case 5:
				mode_pars(client, buffer);
				break;
			default :
				throw std::string("Command does not exist");
		}
	}
	catch (const std::string &e) {
		std::cout << e << std::endl;
		// send(client->_socket_fd, e.c_str(), e.size(), 0);
		// clear buffer
	}
}

bool Server::ping_pars(Client *client, std::string buffer)
{
	(void)buffer;
	std::string pong = "PONG irc.example.com \r\n";
	send(client->_socket_fd, pong.c_str(), pong.size(), 0);
	std::cout << pong << std::endl;
	return (true);
}

void Server::parse(Client *client, std::string buffer)
{
	size_t	bufferSize;

	bufferSize = buffer.size();
	if (buffer[bufferSize - 1] == '\n' && buffer[bufferSize - 2] == '\r')
	{
		if (!client->getRegister())
		{
			set_id(buffer, client);
			std::cout << client->getUsername() << " check = " << client->getCheck() << std::endl;
			if (client->getCheck() == 3)
			{
				std::string welcome = ":irc.example.com 001 "
					+ client->getNickname()
					+ " :Welcome to the IRC Network BRO\r\n";
				send(client->_socket_fd, welcome.c_str(), welcome.size(), 0);
				client->setRegister(true);
				return ;
			}
		}
		else if (client->getRegister())
		{
			std::cout << "BUFFER = " << buffer << std::endl;
			cmd_pars(client, buffer);
			return ;
		}
	}
}
