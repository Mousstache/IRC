/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:20:50 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/27 19:24:10 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void Server::kick_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	size_t channelStart = buffer.find_first_not_of(" \t#\r\n", cmdEnd);
	if (channelStart == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	size_t channelEnd = buffer.find_first_of(" \t", channelStart);
	if (channelEnd == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	std::string channel = buffer.substr(channelStart, channelEnd - channelStart);
	size_t userStart = buffer.find_first_not_of(" \t\r\n", channelEnd);
	if (userStart == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	size_t userEnd = buffer.find_first_of(" \t", userStart);
	if (userEnd == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	std::string user = buffer.substr(userStart, userEnd - userStart);
	std::string reason;
	size_t reasonStart = buffer.find_first_of(":", userEnd);
	if (reasonStart != std::string::npos)
	{
		reasonStart++;
		size_t reasonEnd = buffer.find_first_of("\r\n", reasonStart);
		if (reasonEnd == std::string::npos)
			throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
		reason = buffer.substr(reasonStart, reasonEnd - reasonStart);
	}
	kick_exec(client, channel, user, reason);
}

void Server::kick_exec(Client *client, std::string channel, std::string name, std::string reason)
{
	Channel	*tmp = NULL;
	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	if (tmp == NULL)
	{
		std::string err = ERR_NOSUCHCHANNEL(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(), 0);
		throw std::string("KICK: Channel does not exist !");
	}
	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
	std::map<std::string, Client *>::iterator it3 = admins.find(name);
	std::map<std::string, Client *>::iterator it4 = clients.find(name);

	if (client->getNickname() == name)
	{
		std::string err = NOTICE_CLIENT_INVITE(client->getNickname());
		send(client->getSocket(), err.c_str(), err.size(),0);
        throw std::string("KICK: Client cannot kick himself !");
	}
	if (it1 == admins.end())
	{
		std::string err = ERR_CHANOPRIVSNEED(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(),0);
		throw std::string("KICK: Client cannot kick user if not admin !");
	}
	if (it4 == clients.end())
	{
		std::cout << "JE RENTRE" << std::endl;
		std::string err = ERR_NOTONCHANNEL(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(),0);
		throw std::string("KICK: User not in the channel !");
	}
	if (it3 != admins.end())
		throw std::string("KICK: Client cannot kick an admin user !");
	if (!reason.empty())
	{
		std::cout << "LOOOOOOL" <<std::endl;
		std::string	msg = KICK_CLIENT_REASON(it4->second->getNickname(), it4->second->getUsername(), "KICK", channel, name, reason);
		tmp->chanmsg(msg, "");
	}
	else
	{
		std::string	msg = KICK_CLIENT(client->getNickname(), client->getUsername(), "KICK", channel, name);
		tmp->chanmsg(msg, "");
	}
	tmp->rmClients(name);
}
