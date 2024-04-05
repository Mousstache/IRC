/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:18:53 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/05 19:42:28 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

bool Server::join_pars(Client *client, std::string buffer)
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
		passvalue = buffer.substr(pass, passend - valueStart);
		std::cout << "pass------------------>: " << passvalue << std::endl;
	}
	else
	{
		std::string badchan = ":irc.example.com 432 Vous n'Ãªtes pas sur ce canal.\r\n";
		send(client->_socket_fd, badchan.c_str(), badchan.size(), 0);
		std::cout << "Erreur" << std::endl;
		return (false);
	}
	std::cout << "value " << value << " password  " << passvalue << std::endl;
	return (joinChannel(client, value, passvalue));
}

bool Server::joinChannel(Client *client, std::string channelname,std::string password)
{
	if (client)
	{
		if (_channel.find(channelname) == _channel.end())
		{
            std::cout << "created channel" << std::endl;
			_channel[channelname] = new Channel(client, channelname, password);
			_channel[channelname]->addAdmins(client);
			_channel[channelname]->addClients(client);
			std::string goodjoin = ":" + client->getNickname() + "!"
	            + client->getUsername() + "@localhost JOIN :#" + _channel[channelname]->getName() + "\r\n";
	            send(client->_socket_fd, goodjoin.c_str(), goodjoin.size(), 0);
		}
		else
		{
			if (_channel[channelname]->getNeed() && _channel[channelname]->getPassword() != password)
			{
                std::cout << "NOOOOOO" << std::endl;
                std::string error_passwd = ":ERR_BADCHANNELKEY" +_channel[channelname]->getName() + " :Password incorrect\r\n";
                send(client->_socket_fd, error_passwd.c_str(), error_passwd.size(), 0);
				return false;
			}
				if()
				std::string goodjoin = ":" + client->getNickname() + "!"
	            + client->getUsername() + "@localhost JOIN :#" + _channel[channelname]->getName() + "\r\n";
	            send(client->_socket_fd, goodjoin.c_str(), goodjoin.size(), 0);
            _channel[channelname]->addClients(client);
		}
	}
	_channel[channelname]->print();
	return true;
}
