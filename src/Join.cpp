/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:18:53 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/27 21:43:23 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void Server::join_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	size_t channelStart = buffer.find_first_not_of(" \t#\r\n", cmdEnd);
	if (channelStart == std::string::npos)
		throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	size_t channelEnd = buffer.find_first_of(" \t\n\r", channelStart);
	if (channelEnd == std::string::npos)
		throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	std::string channel = buffer.substr(channelStart, channelEnd - channelStart);
	std::string passvalue;
	size_t passvalueStart = buffer.find_first_not_of(" \t\r\n", channelEnd);
	if (passvalueStart != std::string::npos)
	{
		size_t passvalueEnd = buffer.find_first_of(" \t\r\n", passvalueStart);
		if (passvalueEnd == std::string::npos)
			throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");;
		passvalue = buffer.substr(passvalueStart, passvalueEnd - passvalueStart);
		size_t checkRest = buffer.find_first_not_of(" \t\r\n", passvalueEnd);
    	if (checkRest != std::string::npos)
			throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	}
	joinChannel(client, channel, passvalue);
}

void	Server::joinChannel(Client *client, std::string channelname, std::string password)
{
	std::ostringstream oss;
    oss << std::time(0);
    std::string currentTime = oss.str();
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
			std::string list_client = _channel[channelname]->client_list();
			std::string name = RPL_NAMREPLY(client->getNickname(), _channel[channelname]->getName(), list_client);
	        send(client->_socket_fd, name.c_str(), name.size(), 0);
			std::string end = RPL_ENDOFNAMES(client->getNickname(), _channel[channelname]->getName());
	        send(client->_socket_fd, end.c_str(), end.size(), 0);
		}
		else
		{
			Channel *tmp = _channel[channelname];
            std::map<std::string, Client *> invits = tmp->getInvits();
			std::cout << "Mdp que t'as mis: !" << password << "!" << std::endl;
			if (_channel[channelname]->getNeed() && !tmp->isInvited(client->getNickname()) && _channel[channelname]->getPassword() != password)
			{
                std::cout << "NOOOOOO" << std::endl;
                // std::string error_passwd = ":ERR_BADCHANNELKEY" + tmp->getName() + " :Password incorrect\r\n";
                std::string error_passwd = ERR_BADCHANNELKEY(client->getNickname(), channelname);
				send(client->_socket_fd, error_passwd.c_str(), error_passwd.size(), 0);
				throw std::string("JOIN: Incorrect password !");
			}
            if (tmp->getInvitation() && invits.find(client->getNickname()) == invits.end())
            {
				std::string msg = ERR_INVITEONLYCHAN(client->getNickname(), channelname);
				send(client->getSocket(), msg.c_str(), msg.size(), 0);
                // std::cout << "NOOOOOO" << std::endl;
                throw std::string("JOIN: Client not invited in this channel !");
            }
            if (tmp->getLimit() && (tmp->getLimituser() <= (int)tmp->getClients().size()))
            {
				std::string msg = ERR_CHANNELISFULL(client->getNickname(), channelname);
				send(client->getSocket(), msg.c_str(), msg.size(), 0);
                throw std::string("JOIN: The limit of user has been reached in this channel !");
            }
			else
			{
				std::string goodjoin = ":" + client->getNickname() + "!"
	            + client->getUsername() + "@localhost JOIN :#" + _channel[channelname]->getName() + "\r\n";
	            send(client->_socket_fd, goodjoin.c_str(), goodjoin.size(), 0);
				std::string list_client = tmp->client_list();
				std::cout << "LIST >>>>>>>>> " << list_client << std::endl;
				std::string name = RPL_NAMREPLY(client->getNickname(), tmp->getName(), list_client);
	            send(client->_socket_fd, name.c_str(), name.size(), 0);
				std::string end = RPL_ENDOFNAMES(client->getNickname(), tmp->getName());
	            send(client->_socket_fd, end.c_str(), end.size(), 0);
            	_channel[channelname]->addClients(client);
				if (!tmp->getTopic().empty())
				{
					std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"; 
					std::string err = RPL_TOPIC(client->getNickname(), tmp->getName(), tmp->getTopic());
					// std::string totime = RPL_TOPICWHOTIME(client->getUsername(), tmp->getName(), client->getNickname(), currentTime);
		            send(client->_socket_fd, err.c_str(), err.size(), 0);
		            // send(client->_socket_fd, err.c_str(), err.size(), 0);
				}
			}
		}
	}
	_channel[channelname]->print();
}

	// size_t	i;
	// size_t	cmdStart;
	// size_t	cmdEnd;
	// size_t	valueStart;
	// size_t	valueEnd;
	// size_t	pass;
	// size_t	passend;

	// i = 0;
	// cmdStart = buffer.find_first_of("JOIN", i);
	// std::string passvalue;
	// if (cmdStart == std::string::npos)
	// 	throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	// cmdEnd = buffer.find_first_of(" ", cmdStart);
	// if (cmdEnd == std::string::npos)
	// 	throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	// std::string command = buffer.substr(cmdStart, cmdEnd - cmdStart);
	// std::cout << "Command: " << command << std::endl;
	// valueStart = buffer.find_first_not_of(" #", cmdEnd);
	// if (valueStart == std::string::npos)
	// 	throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	// valueEnd = buffer.find_first_of(" \r\n", valueStart);
	// if (valueEnd == std::string::npos)
	// 	throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	// std::string value = buffer.substr(valueStart, valueEnd - valueStart);
	// std::cout << "Value: " << value << std::endl;
	// if (command != "JOIN")
	// 	throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	// pass = buffer.find_first_not_of(" ", valueEnd);
	// passend = buffer.find_first_of(" \t\r\n", pass);
	// if (passend != std::string::npos && pass != std::string::npos)
	// {
	// 	passvalue = buffer.substr(pass, passend - valueStart);
	// 	std::cout << "pass------------------>: " << passvalue << std::endl;
	// }
	// else
	// {
	// 	std::string badchan = ":irc.example.com 432 Vous n'Ãªtes pas sur ce canal.\r\n";
	// 	send(client->_socket_fd, badchan.c_str(), badchan.size(), 0);
	// 	std::cout << "Erreur" << std::endl;
	// 	throw std::string("JOIN: /JOIN #name_of_the_channel password <-(optionnal)");
	// }
	// std::cout << "value " << value << " password  " << passvalue << std::endl;