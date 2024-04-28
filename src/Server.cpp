/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:21:41 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/28 23:21:17 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h> 

std::string int_to_string(int value) {
    char buffer[20]; // Assez grand pour contenir l'entier converti en chaîne
    std::sprintf(buffer, "%d", value);
    return std::string(buffer);
}

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
	_old_buf = "";
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
	const int enable = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    // error("setsockopt(SO_REUSEADDR) failed");
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
	this->index = 0;
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
		if(_client.size() == 0)
		{
        	_client[0] = new Client(500, client);
        	_client[0]->setNickname("bot");
        	_client[0]->setPassword("bot");
        	_client[0]->setUsername("bot");
        	_client[0]->setRegister(true);
        	FD_SET(0, &rfds);
        	if (0 > max_sd)
        	    max_sd = 0;
    	}
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
                char buffer[1024] = {0};
                int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
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

void Server::exec_cmd(std::string const &command, std::string const &value, Client *client)
{
	if (command == "PASS")
	{
		if (value != this->getPassword())
		{
			// std::cout << "BAD PASSWORD" << std::endl;
			std::string err = "BAD PASSWORD";
			send(client->getSocket(), err.c_str(), err.size(), 0);
			// throw std::string("BAD PASSWORD");
			return ;
		}
		client->setPassword(value);
		client->checkplus();
	}
	else if (command == "NICK")
	{
		for (std::map<int, Client *>::iterator it = _client.begin(); it != _client.end(); ++it)
		{
		    if (it->second->getNickname() == value)
		    {
				// std::string nick = ERR_NICKNAMEINUSE(value);
				// std::cout << "le message : " << nick << std::endl;
				// send(client->getSocket(), nick.c_str(), nick.size(), 0);
				nick_exec(client, "nick" + int_to_string(this->index));
				client->checkplus();
				this->index++;
				return ;
			}
		}
		nick_exec(client, value);
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
		// std::cout << "Command: " << command << std::endl;
		valueStart = str.find_first_not_of(" \r\n", cmdEnd);
		if (valueStart == std::string::npos)
			break ;
		valueEnd = str.find_first_of(" \r\n", valueStart);
		if (valueEnd == std::string::npos)
			break ;
		std::string value = str.substr(valueStart, valueEnd - valueStart);
		// std::cout << "Value: " << value << std::endl;
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
	if (cmd == "NICK")
        return (6);
	if (cmd == "QUIT" || cmd == "PING")
		return (7);
    return (-1);
}

void    Server::cmd_pars(Client *client, std::string buffer)
{
	if (!_old_buf.empty())
	{
		std::string tmp;
		tmp = buffer;
		buffer = _old_buf + tmp;
		if (std::strchr(tmp.c_str(), '\r') != NULL && std::strchr(tmp.c_str(), '\n') != NULL)
			_old_buf = "";
	}
    try {
        size_t    cmdEnd = buffer.find_first_of(" \t\r\n", 0);
        if (cmdEnd == std::string::npos)
            throw std::string("Command does not exist !");
        std::string    cmd = buffer.substr(0, cmdEnd);

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
            case 6:
                nick_pars(client, buffer);
                break;
			case 7:
				ping_pars(client, buffer);
				break;
            default :
                throw std::string("Command does not exist lol!");
        }
    }
    catch (const std::string &e) {
        // std::cout << e << std::endl;
        // send(client->_socket_fd, e.c_str(), e.size(), 0);
        // clear buffer
    }
}

bool Server::ping_pars(Client *client, std::string buffer)
{
	(void)buffer;
	std::string pong = RPL_PONG;
	send(client->_socket_fd, pong.c_str(), pong.size(), 0);
	std::cout << pong << std::endl;
	return (true);
}

void Server::parse(Client *client, std::string buffer)
{
	size_t	bufferSize;
	bufferSize = buffer.size();
	if (buffer[bufferSize - 1] != '\n' && buffer[bufferSize - 2] != '\r')
	{
		if (_old_buf.empty())
			_old_buf = buffer;
		else
			_old_buf += buffer;
	}
	if (!_old_buf.empty())
	{
		std::string tmp;
		tmp = buffer;
		buffer = _old_buf + tmp;
		if (std::strchr(tmp.c_str(), '\r') != NULL && std::strchr(tmp.c_str(), '\n') != NULL)
			_old_buf = "";
	}
	if (std::strchr(buffer.c_str(), '\r') != NULL && std::strchr(buffer.c_str(), '\n') != NULL)
	{
		set_id(buffer, client);
		if (!client->getRegister())
		{
			if (_client.size() >  1)
			{
				// for (std::map<int, Client *>::iterator it = _client.begin(); it != _client.end(); ++it)
				// {
				//     if (it->second)
				//     {
				//         if (it->second->getNickname() == client->getNickname())
				//         {
				// 			std::cout << "nick >> " << it->second->getNickname() << " client  >>" << client->getNickname() << std::endl;
				// 			client->setNickname("nick" + int_to_string(this->index));
				// 			this->index++;
				// 			break ;
				//         }
				//     }
				// }
			}
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
