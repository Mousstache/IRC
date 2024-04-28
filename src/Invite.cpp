/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachera <mvachera@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:19:51 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/26 20:55:53 by mvachera         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void	Server::invite_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("INVITE: /INVITE user #name_of_the_channel");
	size_t userStart = buffer.find_first_not_of(" \t\r\n", cmdEnd);
	if (userStart == std::string::npos)
		throw std::string("INVITE: /INVITE user #name_of_the_channel");
	size_t userEnd = buffer.find_first_of(" \t", userStart);
	if (userEnd == std::string::npos)
		throw std::string("INVITE: /INVITE user #name_of_the_channel");
	std::string user = buffer.substr(userStart, userEnd - userStart);
	size_t channelStart = buffer.find_first_not_of(" \t#\r\n", userEnd);
	if (channelStart == std::string::npos)
		throw std::string("INVITE: /INVITE user #name_of_the_channel");
	size_t channelEnd = buffer.find_first_of(" \t\r\n", channelStart);
	if (channelEnd == std::string::npos)
		throw std::string("INVITE: /INVITE user #name_of_the_channel");
	std::string channel = buffer.substr(channelStart, channelEnd - channelStart);
	size_t checkRest = buffer.find_first_not_of(" \t\r\n", channelEnd);
	if (checkRest != std::string::npos)
		throw std::string("INVITE: /INVITE user #name_of_the_channel");
	invite_exec(client, user, channel);
}

void	Server::invite_exec(Client *client, std::string user, std::string channel)
{
	Channel	*tmp = NULL;
	Client	*tmp2 = NULL;
	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getNickname());
	std::map<std::string, Client *>::iterator it3 = admins.find(user);
	std::map<std::string, Client *>::iterator it4 = clients.find(user);

	std::map<int, Client *>::iterator it = _client.begin();
	while (it != _client.end())
	{
		if (it->second->getNickname() == user)
			tmp2 = it->second;
		it++;
	}

	if (tmp == NULL)
	{
		std::string err = ERR_NOSUCHCHANNEL(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(), 0);
		throw std::string("INVITE: Channel does not exist !");
	}
	if (tmp2 == NULL)
	{
		std::string err = ERR_NOSUCHNICK(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(),0);
		throw std::string("INVITE: User does not exist !");
	}
	if (tmp->getInvitation() == false)
	{
		std::string err = ERR_CHANOPRIVSNEED(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(),0);
        throw std::string("INVITE: You can access this channel without invitation !");
	}
	if (it1 == admins.end() && it2 == clients.end())
		throw std::string("INVITE: Client not in the channel !");
	if (it3 != admins.end() || it4 != clients.end())
	{
		std::string err = ERR_USERONCHANNEL(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(),0);
		throw std::string("INVITE: User already in the channel !");
	}
	
	std::string msg = RPL_INVITING(client->getNickname(), user, channel);
    std::string msg2 = INVITE_CLIENT(client->getNickname(), client->getUsername(), "INVITE ", user, channel);
    send(client->getSocket(), msg.c_str(), msg.size(), 0);
	std::cout << "to send >> " << msg2 << " envoyeur == " << client->getNickname() << "receveur == " << user << "userName == " << client->getUsername() <<  std::endl;
    send(tmp2->getSocket(), msg2.c_str(), msg2.size(), 0);
	std::map<std::string, Client *> invits = tmp->getInvits();
    tmp->addInvits(tmp2);
}
