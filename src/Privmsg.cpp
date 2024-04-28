/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:14:22 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/28 21:40:36 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void	Server::privmsg_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	size_t userStart = buffer.find_first_not_of(" \t\r\n", cmdEnd);
	if (userStart == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	size_t userEnd = buffer.find_first_of(" \t", userStart);
	if (userEnd == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	std::string user = buffer.substr(userStart, userEnd - userStart);
	size_t msgStart = buffer.find_first_not_of(" \t\r\n", userEnd);
	if (msgStart == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	size_t msgEnd = buffer.size();
	while (msgEnd > 0 && (buffer[msgEnd] == ' ' ||  buffer[msgEnd] == '\t'
		|| buffer[msgEnd] == '\r' || buffer[msgEnd] == '\n'))
		msgEnd--;
	std::string	msg = buffer.substr(msgStart + 1, msgEnd - msgStart);
	privmsg_exec(client, user, msg);
}

void	Server::privmsg_exec(Client *client, std::string user, std::string msg)
{
	bool to_channel = false;
	bool send_file = false;
	std::string desti;
	if (!client || user.empty())
		return ;
	if (user[0] == '#')
	{
		desti = user.substr(1, user.size() - 1);
		to_channel = true;
	}
	if (size_t find = msg.find_first_of("DCC") != std::string::npos)
		send_file = true;
	if (send_file)
	{
		//envoie un fichier
	}
	if (msg.size() == 0)
	{
		std::string err = ERR_NOTEXTTOSEND(client->getNickname());
		send(client->getSocket(), err.c_str(), err.size(), 0); 	
	}
	else if (to_channel)
	{
		if (_channel.find(desti) != _channel.end())
		{
    		if (_channel[desti]->getName() == desti)
			{
				if (_channel[desti]->clientExist(client->getNickname()) == false)
				{
					std::string err = ERR_NOTONCHANNEL(client->getNickname(), desti);
					send(client->getSocket(), err.c_str(), err.size(), 0); 	
				}
				else
				{
					std::string bot = msg.substr(0, msg.size() - 2);
					if (bot == "!bot")
					{
						std::string bott = RPL_PRIVMSG_CHANNEL(_client[0]->getNickname(), user, "Coucou c est Christophe tu as recu les photos de ma brique ?!\r\n");
						_channel[desti]->chanmsg(bott, _client[0]->getNickname());
					}
					else
					{
						std::string to_send = RPL_PRIVMSG_CHANNEL(client->getNickname(), user, msg);
						_channel[desti]->chanmsg(to_send, client->getNickname());
						
					}
				}
			}
		}
	}
	else
	{
		if (client->getNickname() == user)
		{
			std::string msg = ERR_NOSUCHNICK(client->getNickname(), user);
			send(client->getSocket(), msg.c_str(), msg.size(), 0);
			throw std::string("PRIVMSG: You cannot send a message to yourself !");
		}
		for (std::map<int, Client *>::iterator it = _client.begin(); it != _client.end(); it++)
    	{
    	    if (it->second->getNickname() == user)
    	    {
				std::string to_send = RPL_PRIVMSG_CLIENT(client->getNickname(), client->getUsername(), user, msg);
				send(it->second->getSocket(), to_send.c_str(), to_send.size(), 0); 
				return ;
    	    }
    	}
		std::string nomatch = ERR_NOSUCHNICK(client->getNickname(), user);
   		send(client->getSocket(), nomatch.c_str(), nomatch.size(), 0);
		throw std::string("PRIVMSG: User does not exist !");
	}
}
